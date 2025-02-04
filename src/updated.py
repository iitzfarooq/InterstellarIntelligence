import math
import random
import pygame
import os
import pickle
from opensimplex import OpenSimplex
import eq
from functools import lru_cache
import equations 

pygame.init()
simplex = OpenSimplex(seed=42)

SCREEN_WIDTH, SCREEN_HEIGHT = 1200, 600 
screenheight = 700
screen = pygame.display.set_mode((SCREEN_WIDTH, screenheight))
pygame.display.set_caption("Planet Habitability Simulation")
clock = pygame.time.Clock()

terrain_cache = {}
planet_radius = min(SCREEN_WIDTH, SCREEN_HEIGHT) // 3

WHITE = (255, 255, 255)
GREEN = (34, 139, 34)
BLUE = (0, 0, 255)
GRAY = (100, 100, 100)
BLACK = (0, 0, 0)
RED = (255, 0, 0)
SANDY = (205, 133, 63)
BLUE1 = (70, 130, 180)  # Water
LIGHT_GREEN = (144, 238, 144)  # Sparse plants
BROWN = (139, 69, 19)  # Dry areas

font = pygame.font.Font(None, 30)

# Independent variables (sliders)
default_variables = {
    "solar_intensity": 0,  
    "humidity": 50,     
    "wind_speed": 10,   
    "population": 1000  
}

# Load saved variables if the file exists
SAVE_FILE = "saved_variables.pkl"
if os.path.exists(SAVE_FILE):
    with open(SAVE_FILE, "rb") as file:
        variables = pickle.load(file)
        print("Variables saved:", variables)
else:
    variables = default_variables.copy()  
def save_variables():
    with open(SAVE_FILE, "wb") as file:
        pickle.dump(variables, file)
    print("Variables saved:", variables)

independent_sliders = [
    {"x": 50, "y": 150, "width": 300, "var": "solar_intensity", "label": "Solar Intensity (W/m²)"},
    {"x": 50, "y": 250, "width": 300, "var": "humidity", "label": "Humidity (%)"},
    {"x": 50, "y": 350, "width": 300, "var": "wind_speed", "label": "Wind Speed (m/s)"},
    {"x": 50, "y": 450, "width": 300, "var": "population", "label": "Population (people/km²)"}
]

def draw_slider(x, y, width, value, label):
    value = max(0, min(100, value))
    value = int(round(value))  # Ensure value stays within the 0-100 range
    pygame.draw.rect(screen, WHITE, (x, y + 3, width, 6))  # Background bar
    handle_x = x + int((value / 100) * width)
    pygame.draw.circle(screen, (224, 180, 74), (handle_x, y + 5), 8)  # Slider knob
    text = font.render(f"{label}: {value:.2f}", True, WHITE)
    screen.blit(text, (x, y - 25))

center_x, center_y = SCREEN_WIDTH // 2, SCREEN_HEIGHT // 2
previous_terrain = {}

solar_intensity = variables["solar_intensity"]#dependent_variables.get("solar_intensity") / 100
@lru_cache(maxsize=None)

def get_terrain_color(noise_value, rainfall, plant_density):
    # Adjust plant density effect
    if plant_density < 50:
        plant_factor = plant_density / 50  # Gradually fades to brown
    elif plant_density > 500:
        plant_factor = max(0, (200 - plant_density) / 100)#max(0, 1 - (plant_density - 100) / 100)  # Gradually turns brown again
    else:
        plant_factor = 1  # Most green in 50-100 range

    # Adjust rainfall effect
    if rainfall < 80000:
        rain_factor = 0  # No blue for very low rainfall
    elif rainfall > 90000:
        rain_factor = max(0, 1 - (rainfall - 90000) / 10000)  # Gradually fades out above 90000
    else:
        rain_factor = (rainfall - 80000) / (90000 - 80000)  # Most blue in 80000-90000 range

    if noise_value < -0.1:
        transition = plant_factor  # Use adjusted plant density
        r = int(BROWN[0] * (1 - transition) + GREEN[0] * transition)
        g = int(BROWN[1] * (1 - transition) + GREEN[1] * transition)
        b = int(BROWN[2] * (1 - transition) + GREEN[2] * transition)
    elif noise_value < 0:
        transition = rain_factor  # Use adjusted rainfall
        r = int(SANDY[0] * (1 - transition) + LIGHT_GREEN[0] * transition)
        g = int(SANDY[1] * (1 - transition) + LIGHT_GREEN[1] * transition)
        b = int(SANDY[2] * (1 - transition) + LIGHT_GREEN[2] * transition)
    else:
        transition = rain_factor  # Use adjusted rainfall
        r = int(BROWN[0] * (1 - transition) + BLUE1[0] * transition)
        g = int(BROWN[1] * (1 - transition) + BLUE1[1] * transition)
        b = int(BROWN[2] * (1 - transition) + BLUE1[2] * transition)

    return (max(0, min(255, r)), max(0, min(255, g)), max(0, min(255, b)))

noise_map = {}

def get_noise_value(x, y):
    if (x, y) in noise_map:
        return noise_map[(x, y)]
    
    noise_value = simplex.noise2(x / 50, y / 50) + 0.5 * simplex.noise2(x / 30, y / 30) + 0.25 * simplex.noise2(x / 10, y / 10)
    
    noise_map[(x, y)] = noise_value
    return noise_value


CLOUD_COLOR = (255, 255, 255, 60) 
cloud_noise_offset = 0  

def draw_clouds(radius, cloud_density):
    global cloud_noise_offset
    cloud_surface = pygame.Surface((SCREEN_WIDTH, SCREEN_HEIGHT), pygame.SRCALPHA)

    adjusted_radius = min(radius, planet_radius)  
    if cloud_density < 100:
        cloud_opacity = 5
    elif cloud_density < 150:
        cloud_opacity = 50
    elif cloud_density < 200:
        cloud_opacity = 60
    elif cloud_density < 400:
        cloud_opacity = 70
    elif cloud_density < 1000:
        cloud_opacity = 90
    elif cloud_density < 2000:
        cloud_opacity = 120
    elif cloud_density < 3000:
        cloud_opacity = 140
    elif cloud_density < 6000:
        cloud_opacity = 160
    else:
        cloud_opacity = 170 # Best whiteness at 600+

    cloud_color = (255, 255, 255, cloud_opacity)  

    for y in range(int(center_y - adjusted_radius), int(center_y + adjusted_radius), 5):
        for x in range(int(center_x - adjusted_radius), int(center_x + adjusted_radius), 5):
            distance = math.sqrt((x - center_x) ** 2 + (y - center_y) ** 2)
            if distance <= adjusted_radius:
                noise_value = simplex.noise2((x + cloud_noise_offset) / 80, (y + cloud_noise_offset) / 80)
                if noise_value > 0.2:  
                    pygame.draw.circle(cloud_surface, cloud_color, (x, y), 5)

    screen.blit(cloud_surface, (0, 0))
    cloud_noise_offset += variables["wind_speed"] * 0.2  # Wind effect

def draw_shading_overlay(radius, solar_intensity):
    if solar_intensity < 20:
        darkness = 180  # Very light shading
    elif 20 <= solar_intensity < 40:
        darkness = 100  # Light shading
    elif 40 <= solar_intensity < 60:
        darkness = 70  # Moderate shading
    elif 60 <= solar_intensity < 80:
        darkness = 60  # Darker shading
    else:
        darkness = 30  # Very dark shading

    shading_surface = pygame.Surface((2 * radius, 2 * radius), pygame.SRCALPHA)
    pygame.draw.circle(shading_surface, (0, 0, 0, darkness), (radius, radius), radius)

    screen.blit(shading_surface, (center_x - radius, center_y - radius))

def draw_planet(radius, rainfall, plant_density, asi, cloud_density):
    center_x, center_y = SCREEN_WIDTH // 2, SCREEN_HEIGHT // 2
    max_glow_alpha = min(255, 100 + int(asi * 1.55))

    if asi < 5000:
        glow_r = min(255, 200 + int(asi * 0.2))  
        glow_g = min(255, 200 + int(asi * 0.2))
        glow_b = min(255, 220 + int(asi * 0.25))
        layer_spacing = 15  # Fewer layers
    elif 6000 <= asi <= 9000:
        glow_r = min(255, 240 + int(asi * 0.3))  
        glow_g = min(255, 240 + int(asi * 0.3))
        glow_b = min(255, 240 + int(asi * 0.3))
        layer_spacing = 10  
    else:
        glow_r = min(255, 250 + int(asi * 0.2))  
        glow_g = min(255, 250 + int(asi * 0.2))
        glow_b = min(255, 255)
        layer_spacing = 17 

    for i in range(radius + 5, radius + 30, layer_spacing):  
        alpha = max(0, max_glow_alpha - (i - radius) * 5)  # Fade effect
        glow_color = (glow_r, glow_g, glow_b, alpha)
        glow_surface = pygame.Surface((SCREEN_WIDTH, SCREEN_HEIGHT), pygame.SRCALPHA)
        pygame.draw.circle(glow_surface, glow_color, (center_x, center_y), i)
        screen.blit(glow_surface, (0, 0))

    for y in range(center_y - radius, center_y + radius, 3):
        for x in range(center_x - radius, center_x + radius, 3):
            distance = math.sqrt((x - center_x) ** 2 + (y - center_y) ** 2)
            if distance <= radius:
                noise_value = get_noise_value(x, y)  
                color = get_terrain_color(noise_value, rainfall, plant_density)
                pygame.draw.rect(screen, color, (x, y, 3, 3))

    draw_shading_overlay(radius, variables["solar_intensity"])
    draw_clouds(radius, cloud_density)


def draw_dependent_variables(dependent_variables):
    y_offset = 80
    bar_width = 250
    x_offset = SCREEN_WIDTH - 350
    vertical_spacing = 30

    for key, value in dependent_variables.items():
        if y_offset + vertical_spacing > screenheight - 50:
            break
        draw_horizontal_bar(x_offset, y_offset, bar_width, value, key)
        y_offset += vertical_spacing


def draw_horizontal_bar(x, y, width, value, label):
    bar_height = 5
    bar_value = int(min(width, max(0, (value / 100) * width)))
    pygame.draw.rect(screen, GRAY, (x, y, width, bar_height), border_radius=3)
    pygame.draw.rect(screen, (224, 180, 74), (x, y, bar_value, bar_height), border_radius=3)
    text = font.render(f"{label}: {value:.2f}", True, WHITE)
    screen.blit(text, (x, y - 20))


def reset_variables():
    global variables
    variables = default_variables.copy()

def draw_button(x, y, width, height, text, color, hover_color, is_hovering):
    button_color = hover_color if is_hovering else color
    pygame.draw.rect(screen, button_color, (x, y, width, height))
    text_surface = font.render(text, True, WHITE)
    text_x = x + (width - text_surface.get_width()) // 2
    text_y = y + (height - text_surface.get_height()) // 2
    screen.blit(text_surface, (text_x, text_y))

num_stars = 120
stars = [(random.randint(0, SCREEN_WIDTH), random.randint(0, screenheight), random.uniform(0.5, 2), random.randint(1, 3)) for _ in range(num_stars)]

def draw_stars():
    for i in range(len(stars)):
        x, y, speed, size = stars[i]
        
        if random.random() < 0.08:  
            size = random.randint(1, 3)
        
        pygame.draw.circle(screen, WHITE, (int(x), y), size)

        x = (x + 1) % SCREEN_WIDTH
        stars[i] = (x, y, speed, size)  # U


running = True
dragging_slider = None
while running:
    screen.fill(BLACK)
    draw_stars() 
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
        elif event.type == pygame.MOUSEBUTTONDOWN:
            for slider in independent_sliders:
                x, y, width, var, _ = slider.values()
                if x <= event.pos[0] <= x + width and y - 10 <= event.pos[1] <= y + 20:
                    dragging_slider = slider
                if 50 <= event.pos[0] <= 170 and 500 <= event.pos[1] <= 540:
                    reset_variables()
                if 200 <= event.pos[0] <= 320 and 500 <= event.pos[1] <= 540:
                    save_variables()

        elif event.type == pygame.MOUSEBUTTONUP:
            dragging_slider = None

        elif event.type == pygame.MOUSEMOTION and dragging_slider:
            x, y, width, var, _ = dragging_slider.values()
            relative_x = event.pos[0] - x
            value = max(0, min(100, (relative_x / width) * 100))
            variables[var] = value

    dependent_variables = equations.calculate_dependent_variables(variables)

    plants_density = max(0, min(100, dependent_variables.get("plants_density"))) 
    rainfall_area = dependent_variables.get("rainfall_area") 
    asi = dependent_variables.get("asi") 
    cloud_density = int(dependent_variables.get("cloud_density"))
    rainfall_intensity = dependent_variables.get("rainfall_intensity")

    draw_planet(200, rainfall_area,plants_density, asi, cloud_density)

    for slider in independent_sliders:
        draw_slider(slider["x"], slider["y"], slider["width"], variables[slider["var"]], slider["label"])

    draw_dependent_variables(dependent_variables)

    mouse_pos = pygame.mouse.get_pos()
    is_hovering_default = 50 <= mouse_pos[0] <= 170 and 500 <= mouse_pos[1] <= 540
    is_hovering_save = 200 <= mouse_pos[0] <= 320 and 500 <= event.pos[1] <= 540
    draw_button(50, 500, 120, 40, "Default", GRAY, (194, 197, 204), is_hovering_default)
    draw_button(200, 500, 120, 40, "Save", GREEN, (100, 255, 100), is_hovering_save)

    pygame.display.flip()
    clock.tick(30)

pygame.quit()
