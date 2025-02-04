import math

def clamp(value, min_val, max_val):
    if value < min_val:
        return min_val
    if value > max_val:
        return max_val
    return value

def calculate_temperature(humidity, solar_intensity):
    temp = 0.02 * humidity + solar_intensity
    if temp < 0.0:
        return 0.0
    if temp > 102.0:
        return 102.0
    return temp

def calculate_cloud_density(humidity, solar_intensity):
    cloud_density = (humidity**2) / max(solar_intensity, 1)
    if cloud_density < 0.0:
        return 0.0
    if cloud_density > 10000.0:
        return 10000.0
    return cloud_density

def calculate_photosynthesis(temperature, cloud_density):
    photosynthesis = temperature * (0.5 + 0.5 * math.cos(cloud_density))
    if photosynthesis < 0:
        return 0
    if photosynthesis > 101:
        return 101
    return photosynthesis

def calculate_plants_density(solar_intensity, photosynthesis):
    plants_density = solar_intensity**2 / 10 + photosynthesis
    if plants_density < 0.0:
        return 0.0
    if plants_density > 1101.31:
        return 1101.31
    return plants_density

def calculate_oxygen(photosynthesis, plants_density, population):
    oxygen = 5 + 1.5 * photosynthesis + plants_density**2 - 0.05 * population
    if oxygen < 0.0:
        return 0.0
    if oxygen > 1213028.81:
        return 1213028.81
    return oxygen

def calculate_carbon_dioxide(photosynthesis, population):
    carbon_dioxide = 40 + 10 * population - 0.005 * photosynthesis
    if carbon_dioxide < 39.49:
        return 39.49
    if carbon_dioxide > 1040.0:
        return 1040.0
    return carbon_dioxide

def calculate_asi(oxygen, carbon_dioxide):
    asi = math.sqrt(oxygen**2 + carbon_dioxide**2)
    if asi < 39.99:
        return 39.99
    if asi > 1213028.81:
        return 1213028.81
    return asi

def calculate_rainfall_intensity(humidity, solar_intensity, wind_speed):
    rainfall_intensity = 0.1 * humidity * solar_intensity * (1 + wind_speed / 100)
    if rainfall_intensity < 0.0:
        return 0.0
    if rainfall_intensity > 2000.0:
        return 2000.0
    return rainfall_intensity

def calculate_radius_of_wet_ground(rainfall_intensity, wind_speed):
    radius = rainfall_intensity * wind_speed
    if radius < 0.0:
        return 0.0
    if radius > 200000.0:
        return 200000.0
    return radius

def calculate_rainfall_area(radius_of_wet_ground):
    area = math.pi * radius_of_wet_ground * 2
    if area < 0.0:
        return 0.0
    if area > 1256637.06:
        return 1256637.06
    return area

def calculate_power(temperature, wind_speed):
    power = temperature**2 + wind_speed
    if power < 0.0:
        return 0.0
    if power > 10504.0:
        return 10504.0
    return power

def calculate_uv_index(temperature, solar_intensity):
    uv_index = 0.01 * temperature * solar_intensity
    if uv_index < 0.0:
        return 0.0
    if uv_index > 102.0:
        return 102.0
    return uv_index

def calculate_pollution(population, wind_speed):
    pollution = 10 * population + 0.005 * wind_speed
    if pollution < 0.0:
        return 0.0
    if pollution > 1000.5:
        return 1000.5
    return pollution

def calculate_health_risk(uv_index, pollution):
    health_risk = math.log(1 + uv_index + pollution)
    if health_risk < 0.0:
        return 0.0
    if health_risk > 11.53:
        return 11.53
    return health_risk

def calculate_crop_yield(solar_intensity, humidity, plants_density):
    crop_yield = 0.05 * (solar_intensity - 20) * humidity * plants_density if solar_intensity > 20 else 0
    if crop_yield < 0:
        return 0
    if crop_yield > 437991.30:
        return 437991.30
    return crop_yield

def calculate_hunger(population, crop_yield):
    hunger = population / max(crop_yield, 1)  
    if hunger < 0.0:
        return 0.0
    if hunger > 100.0:
        return 100.0
    return hunger

def calculate_water_resources(rainfall_intensity, wind_speed, population):
    water_resources = 10 + rainfall_intensity + 0.2 * wind_speed - 0.05 * population
    if water_resources < 5.0:
        return 5.0
    if water_resources > 2030.0:
        return 2030.0
    return water_resources

def calculate_thirst(population, rainfall_area):
    thirst = population / max(rainfall_area, 1)  
    if thirst < 0.0:
        return 0.0
    if thirst > 100.0:
        return 100.0
    return thirst

def calculate_dependent_variables(variables):
    solar_intensity = variables["solar_intensity"]
    humidity = variables["humidity"]
    wind_speed = variables["wind_speed"]
    population = variables["population"]
    
    temperature = calculate_temperature(humidity, solar_intensity)
    cloud_density = calculate_cloud_density(humidity, solar_intensity)
    photosynthesis = calculate_photosynthesis(temperature, cloud_density)
    plants_density = calculate_plants_density(solar_intensity, photosynthesis)
    oxygen = calculate_oxygen(photosynthesis, plants_density, population)
    carbon_dioxide = calculate_carbon_dioxide(photosynthesis, population)
    asi = calculate_asi(oxygen, carbon_dioxide)
    rainfall_intensity = calculate_rainfall_intensity(humidity, solar_intensity, wind_speed)
    radius_of_wet_ground = calculate_radius_of_wet_ground(rainfall_intensity, wind_speed)
    rainfall_area = calculate_rainfall_area(radius_of_wet_ground)
    power = calculate_power(temperature, wind_speed)
    uv_index = calculate_uv_index(temperature, solar_intensity)
    pollution = calculate_pollution(population, wind_speed)
    health_risk = calculate_health_risk(uv_index, pollution)
    crop_yield = calculate_crop_yield(solar_intensity, humidity, plants_density)
    hunger = calculate_hunger(population, crop_yield)
    water_resources = calculate_water_resources(rainfall_intensity, wind_speed, population)
    thirst = calculate_thirst(population, rainfall_area)
    
    return {
        "temperature": int(temperature),
        "cloud_density": int(cloud_density),
        "photosynthesis": int(photosynthesis),
        "oxygen": int(oxygen),  
        "carbon_dioxide": int(carbon_dioxide), 
        "asi": int(asi),
        "rainfall_intensity": int(rainfall_intensity),
        "radius_of_wet_ground": int(radius_of_wet_ground),
        "rainfall_area": int(rainfall_area),
        "power": int(power),
        "uv_index": int(uv_index),
        "pollution": int(pollution),
        "health_risk": int(health_risk),
        "plants_density": int(plants_density),
        "crop_yield": int(crop_yield),
        "hunger": int(hunger),
        "water_resources": int(water_resources),
        "thirst": int(thirst)
    }