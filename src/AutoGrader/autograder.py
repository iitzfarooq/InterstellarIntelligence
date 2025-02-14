import os, sys
import numpy as np
import math, pytest

FILE_DIR = os.path.dirname(os.path.abspath(__file__))

def update_path():
    """
    Sets the path for submission directory. This is necessary to
    import submission.py in Github workflow.
    """

    directory = os.path.dirname(os.path.abspath(__file__))
    submission_repo = os.path.abspath(os.path.join(directory, ".."))

    if submission_repo not in sys.path:
        sys.path.insert(0, submission_repo)


update_path()

import submission
import calculation
import actual_solution

class Grader:
    def __init__(self, config, submission, actual_solution, calculation_module):
        self.config = config
        self.submission = submission
        self.actual_solution = actual_solution
        self.calculation_module = calculation_module
        
        self.indep_vars = config.get('indep_vars', ['solar_intensity', 'humidity', 'wind_speed', 'population'])
        self.nindep_vars = len(self.indep_vars)
        self.ntrials = config.get('ntrials', 50)
        self.min_val, self.max_val = config.get('min_max', (0, 100))
        
        self.weights = config['weights']
        self.scaling = config['scaling']
        
        self.sub_evalOrder = [key for key, _ in submission.evaluation_order]
        self.dep_vars = len(self.sub_evalOrder)
        
    def generate_trials(self):
        """ 
        Generates a ntrials of vars-tuple in a list. Each combination is unique
        and has the equal probability. The values will be float.
        """
        trials = np.random.uniform(self.min_val, self.max_val, (self.ntrials, self.nindep_vars))
        unique_trials = np.unique(trials, axis=0)

        while unique_trials.shape[0] < self.ntrials:
            missing = self.ntrials - unique_trials.shape[0]
            more_trials = np.random.uniform(self.min_val, self.max_val, (missing, self.nindep_vars))
            trials = np.vstack([unique_trials, more_trials])
            unique_trials = np.unique(trials, axis=0)

        return [tuple(row) for row in unique_trials.tolist()]
    
    def get_weight_vector(self):
        """
        Generates a weight vector where wi is the weight corresponding to the
        variable in the evaluation order.
        """
        n = self.dep_vars
        assert n != 0

        return np.array([[self.weights[variable]] for variable in self.submission.evaluation_order])
    
    @staticmethod
    def calculate_error(computed, actual, scaling):
        """
        Calculates the error between computed and actual values using a scaling factor.
        """
        abs_diff = abs(computed - actual)
        scaling = max(scaling, 1)
        return 1 - math.exp(-abs_diff / scaling)

    def build_engines(self):
        """
        Builds the engines for submission and actual solution.
        """
        self.sub_engine = self.calculation_module.build_engine(self.submission.evaluation_order)
        self.act_engine = self.calculation_module.build_engine(self.actual_solution.evaluation_order)
        
    def compute_grade(self):
        """
        Grades the submission by comparing the computed values with the actual values
        over a number of trials.
        """
        trials = self.generate_trials()
        self.build_engines()
        matrix = np.zeros((self.dep_vars, self.ntrials))  # initialize a matrix of order (18 x ntrials)
        weight = self.get_weight_vector()
        
        variables = {}
        for j, trial in enumerate(trials):
            for i in range(self.nindep_vars):
                variables[self.indep_vars[i]] = trial[i]
            try:    
                computed = self.sub_engine.compute(variables)
                actual = self.act_engine.compute(variables)
            except Exception as e:
                matrix[i, j] = 1
                
            for i, var in enumerate(self.sub_evalOrder):
                cmp = computed.get(var)
                act = actual.get(var)
                scale = self.scaling.get(var)

                matrix[i, j] = self.calculate_error(cmp, act, scale)
                
        error_vector = np.mean(matrix, axis=1)
        net_error = error_vector @ weight
        final_grade = 1 - net_error.item()
        return final_grade * 100
    
def grade():
    config = {
        "indep_vars": ['solar_intensity', 'humidity', 'wind_speed', 'population'],
        "ntrials": 50,
        "min_max": (0, 100),
        "weights": {
            "temperature": 1 / 18,
            "cloud_density": 1 / 18,
            "photosynthesis": 1 / 18,
            "plants_density": 1 / 18,
            "oxygen": 1 / 18,
            "carbon_dioxide": 1 / 18,
            "asi": 1 / 18,
            "rainfall_intensity": 1 / 18,
            "radius_of_wet_ground": 1 / 18,
            "rainfall_area": 1 / 18,
            "power": 1 / 18,
            "uv_index": 1 / 18,
            "pollution": 1 / 18,
            "health_risk": 1 / 18,
            "crop_yield": 1 / 18,
            "hunger": 1 / 18,
            "water_resources": 1 / 18,
            "thirst": 1 / 18,
        },
        "scaling": {var: 0.01* (r[1] - r[0]) for var, r in actual_solution.variable_ranges.items()}
    }
    
    grader = Grader(config, submission, actual_solution, calculation)
    return grader.compute_grade()


if __name__ == "__main__":
    status = pytest.main(['-q', os.path.join(FILE_DIR, 'test.py')])
    if status != 0:
        assert False, "Test failed"
        
    grade_value = grade()