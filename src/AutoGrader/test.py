import os
import sys

import pytest

FILE_DIR = os.path.dirname(os.path.abspath(__file__))


def update_path():
    """
    Sets the path for submission directory. This is necessary to
    import submission.py in Github workflow.
    """

    directory = FILE_DIR
    parent = os.path.abspath(os.path.join(directory, ".."))

    if parent not in sys.path:
        sys.path.insert(0, parent)


update_path()

import submission
import calculation


def test_path():
    parent = os.path.abspath(os.path.join(FILE_DIR, ".."))

    submission_file = os.path.join(parent, "submission.py")
    calculation_file = os.path.join(parent, "calculation.py")

    assert os.path.exists(submission_file), f"submission.py not found in {parent}"
    assert os.path.exists(calculation_file), f"calculation.py not found in {parent}"


variable_strings = [
    "temperature",
    "cloud_density",
    "photosynthesis",
    "plants_density",
    "oxygen",
    "carbon_dioxide",
    "asi",
    "rainfall_intensity",
    "radius_of_wet_ground",
    "rainfall_area",
    "power",
    "uv_index",
    "pollution",
    "health_risk",
    "crop_yield",
    "hunger",
    "water_resources",
    "thirst",
]


def test_computation():
    assert hasattr(
        submission, "evaluation_order"
    ), "evaluation_order not found in submission.py"

    indep_vars = {
        "solar_intensity": 0, 
        "humidity": 0,
        "wind_speed": 0, 
        "population": 0
    }
    
    engine = calculation.build_engine(submission.evaluation_order)
    vars = None
    try:
        vars = engine.compute(indep_vars)
    except Exception as e:
        assert False, f"Error in compute method: {e}"
        
    assert vars is not None, "No variables computed"
    assert vars is not indep_vars, "indep_vars should not be modified"
        
    for variable in variable_strings:
        assert variable in vars, f"{variable} not found in computed variables"
        
    assert len(vars) == 18, "Incorrect number of variables computed"
