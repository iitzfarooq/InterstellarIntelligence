import math


def calculate_temperature(variables):
    humidity = variables["humidity"]
    solar_intensity = variables["solar_intensity"]
    return 0.02 * humidity + solar_intensity


def calculate_cloud_density(variables):
    humidity = variables["humidity"]
    solar_intensity = variables["solar_intensity"]
    return (humidity**2) / max(solar_intensity, 1)


def calculate_photosynthesis(variables):
    temperature = calculate_temperature(variables)
    cloud_density = calculate_cloud_density(variables)
    return temperature * (0.5 + 0.5 * math.cos(cloud_density))


def calculate_plants_density(variables):
    solar_intensity = variables["solar_intensity"]
    photosynthesis = calculate_photosynthesis(variables)
    return (solar_intensity**2) / 10 + photosynthesis


def calculate_oxygen(variables):
    photosynthesis = calculate_photosynthesis(variables)
    plants_density = calculate_plants_density(variables)
    population = variables["population"]
    return 5 + 1.5 * photosynthesis + (plants_density**2) - 0.05 * population


def calculate_carbon_dioxide(variables):
    population = variables["population"]
    photosynthesis = calculate_photosynthesis(variables)
    return 40 + 10 * population - 0.005 * photosynthesis


def calculate_asi(variables):
    oxygen = calculate_oxygen(variables)
    carbon_dioxide = calculate_carbon_dioxide(variables)
    return math.sqrt(oxygen**2 + carbon_dioxide**2)


def calculate_rainfall_intensity(variables):
    humidity = variables["humidity"]
    solar_intensity = variables["solar_intensity"]
    wind_speed = variables["wind_speed"]
    return 0.1 * humidity * solar_intensity * (1 + wind_speed / 100)


def calculate_radius_of_wet_ground(variables):
    wind_speed = variables["wind_speed"]
    rainfall_intensity = calculate_rainfall_intensity(variables)
    return rainfall_intensity * wind_speed


def calculate_rainfall_area(variables):
    radius_of_wet_ground = calculate_radius_of_wet_ground(variables)
    return math.pi * radius_of_wet_ground * 2


def calculate_power(variables):
    wind_speed = variables["wind_speed"]
    temperature = calculate_temperature(variables)
    return temperature**2 + wind_speed


def calculate_uv_index(variables):
    temperature = calculate_temperature(variables)
    solar_intensity = variables["solar_intensity"]
    return 0.01 * temperature * solar_intensity


def calculate_pollution(variables):
    population = variables["population"]
    wind_speed = variables["wind_speed"]
    return 10 * population + 0.005 * wind_speed


def calculate_health_risk(variables):
    uv_index = calculate_uv_index(variables)
    pollution = calculate_pollution(variables)
    return math.log(1 + uv_index + pollution)


def calculate_crop_yield(variables):
    solar_intensity = variables["solar_intensity"]
    humidity = variables["humidity"]
    plants_density = calculate_plants_density(variables)
    if solar_intensity > 20:
        return 0.05 * (solar_intensity - 20) * humidity * plants_density
    else:
        return 0


def calculate_hunger(variables):
    population = variables["population"]
    crop_yield = calculate_crop_yield(variables)
    return population / max(crop_yield, 1)


def calculate_water_resources(variables):
    rainfall_intensity = calculate_rainfall_intensity(variables)
    wind_speed = variables["wind_speed"]
    population = variables["population"]
    return 10 + rainfall_intensity + 0.2 * wind_speed - 0.05 * population


def calculate_thirst(variables):
    population = variables["population"]
    rainfall_area = calculate_rainfall_area(variables)
    return population / max(rainfall_area, 1)


evaluation_order = [
    ("temperature", calculate_temperature),
    ("cloud_density", calculate_cloud_density),
    ("photosynthesis", calculate_photosynthesis),
    ("plants_density", calculate_plants_density),
    ("oxygen", calculate_oxygen),
    ("carbon_dioxide", calculate_carbon_dioxide),
    ("asi", calculate_asi),
    ("rainfall_intensity", calculate_rainfall_intensity),
    ("radius_of_wet_ground", calculate_radius_of_wet_ground),
    ("rainfall_area", calculate_rainfall_area),
    ("power", calculate_power),
    ("uv_index", calculate_uv_index),
    ("pollution", calculate_pollution),
    ("health_risk", calculate_health_risk),
    ("crop_yield", calculate_crop_yield),
    ("hunger", calculate_hunger),
    ("water_resources", calculate_water_resources),
    ("thirst", calculate_thirst),
]

vars = len(evaluation_order)

weights = dict(
    [
        ("temperature", 1 / vars),
        ("cloud_density", 1 / vars),
        ("photosynthesis", 1 / vars),
        ("plants_density", 1 / vars),
        ("oxygen", 1 / vars),
        ("carbon_dioxide", 1 / vars),
        ("asi", 1 / vars),
        ("rainfall_intensity", 1 / vars),
        ("radius_of_wet_ground", 1 / vars),
        ("rainfall_area", 1 / vars),
        ("power", 1 / vars),
        ("uv_index", 1 / vars),
        ("pollution", 1 / vars),
        ("health_risk", 1 / vars),
        ("crop_yield", 1 / vars),
        ("hunger", 1 / vars),
        ("water_resources", 1 / vars),
        ("thirst", 1 / vars),
    ]
)

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

scaling = {variable: 0.01 * (r[1] - r[0]) for variable, r in variable_ranges.items()}