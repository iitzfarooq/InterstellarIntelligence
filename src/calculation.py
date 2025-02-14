import submission


class Engine:
    def __init__(self):
        """
        Initialize the engine with function map and evaluation order
        """
        self.functions = {}
        self.evaluation_order = []

    def add_function(self, name, func):
        """
        Add function to the evaluation order against the name

        Args:
            name (string): name of variable/function (rainfall etc.)
            func (function): function taking map of variables
        """

        self.functions[name] = func
        self.evaluation_order.append(name)

    def compute(self, indep_variables):
        """
        Computes the values of functions according to their evaluation order
        and maps them against their names

        Args:
            indep_variables (map): name(str) to value(int/float) map

        Returns:
            map: name(str) to value(int/float) map
        """

        variables = indep_variables.copy()

        for name in self.evaluation_order:
            variables[name] = self.functions[name](variables)

        return variables


class Normalizer:
    clamp = lambda value, min_val, max_val: min(max(value, min_val), max_val)

    def __init__(self, ranges):
        """
        Initializes the ranges dictionary
        Args:
            ranges (dict): variable names to (min, max) tuples.
        """
        self.ranges = ranges

    def clamp_all(self, computed):
        """
        Clamps all the values in the computed dictionary
        Args:
            computed (dict): variable names to int/float values

        Returns:
            dict: variable names to clamped values
        """
        clamped = {}

        for key, val in computed.items():
            if key in self.ranges:
                min_val, max_val = self.ranges[key]
                clamped[key] = Normalizer.clamp(val, min_val, max_val)
            else:
                clamped[key] = val

        return clamped

    def normalize(self, computed):
        """
        Normalizes the computed values. Uses clamping technique.
        Args:
            computed (dict): variable names to int/float values

        Returns:
            dict: variable names to normalized values (in the range [0, 1])
        """

        normalized = {}

        for key, val in computed:
            if key in self.ranges:
                min_val, max_val = self.ranges[key]

                Normalizer.clamp(val, min_val, max_val)
                val = (val - min_val) / (max_val - min_val)

            normalized[key] = val

        return normalized


def build_engine(order=None):
    """
    Builds the engine based on the evaluation order (by default from the equations.py)
    Returns: Engine Object
    """
    if order is None:
        order = submission.evaluation_order

    engine = Engine()

    for key, func in order:
        engine.add_function(key, func)

    return engine


variable_ranges = {
    "temperature": (0.0, 102.0),
    "cloud_density": (0.0, 10000.0),
    "photosynthesis": (0, 101),
    "plants_density": (0.0, 1101.31),
    "oxygen": (0.0, 1213028.81),
    "carbon_dioxide": (39.49, 1040.0),
    "asi": (39.99, 1213028.81),
    "rainfall_intensity": (0.0, 2000.0),
    "radius_of_wet_ground": (0.0, 200000.0),
    "rainfall_area": (0.0, 1256637.06),
    "power": (0.0, 10504.0),
    "uv_index": (0.0, 102.0),
    "pollution": (0.0, 1000.5),
    "health_risk": (0.0, 11.53),
    "crop_yield": (0, 437991.30),
    "hunger": (0.0, 100.0),
    "water_resources": (5.0, 2030.0),
    "thirst": (0.0, 100.0),
}


def build_normalizer(ranges=None):
    if ranges is None:
        ranges = variable_ranges

    return Normalizer(ranges)


def compute_and_normalize(engine, normalizer, indep_variables):
    """
    Computes the values of functions according to their evaluation order
    and maps them against their names

    Args:
        engine (Engine): Engine object
        normalizer (Normalizer): Normalizer object
        indep_variables (map): name(str) to value(int/float) map

    Returns:
        map: name(str) to normalized value(float) map
    """
    computed = engine.compute(indep_variables)
    return normalizer.normalize(computed)
