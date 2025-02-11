import math

# NO MORE IMPORTS ALLOWED!
################# WRITE BELOW THIS #################

def calculate_temperature(variables):
    raise NotImplementedError

def calculate_cloud_density(variables):
    raise NotImplementedError

def calculate_photosynthesis(variables):
    raise NotImplementedError

def calculate_plants_density(variables):
    raise NotImplementedError

def calculate_oxygen(variables):
    raise NotImplementedError

def calculate_carbon_dioxide(variables):
    raise NotImplementedError

def calculate_asi(variables):
    raise NotImplementedError

def calculate_rainfall_intensity(variables):
    raise NotImplementedError

def calculate_radius_of_wet_ground(variables):
    raise NotImplementedError

def calculate_rainfall_area(variables):
    raise NotImplementedError

def calculate_power(variables):
    raise NotImplementedError

def calculate_uv_index(variables):
    raise NotImplementedError

def calculate_pollution(variables):
    raise NotImplementedError

def calculate_health_risk(variables):
    raise NotImplementedError

def calculate_crop_yield(variables):
    raise NotImplementedError

def calculate_hunger(variables):
    raise NotImplementedError

def calculate_water_resources(variables):
    raise NotImplementedError

def calculate_thirst(variables):
    raise NotImplementedError

# Reorder the sequence as you like (do not change the strings)

evaluation_order = [
    ('temperature', calculate_temperature),
    ('cloud_density', calculate_cloud_density),
    ('photosynthesis', calculate_photosynthesis),
    ('oxygen', calculate_oxygen),
    ('carbon_dioxide', calculate_carbon_dioxide),
    ('asi', calculate_asi),
    ('rainfall_intensity', calculate_rainfall_intensity),
    ('radius_of_wet_ground', calculate_radius_of_wet_ground),
    ('rainfall_area', calculate_rainfall_area),
    ('power', calculate_power),
    ('uv_index', calculate_uv_index),
    ('pollution', calculate_pollution),
    ('health_risk', calculate_health_risk),
    ('plants_density', calculate_plants_density),
    ('crop_yield', calculate_crop_yield),
    ('hunger', calculate_hunger),
    ('water_resources', calculate_water_resources),
    ('thirst', calculate_thirst)
]