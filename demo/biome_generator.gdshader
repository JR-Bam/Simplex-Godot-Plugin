shader_type canvas_item;

uniform sampler2D elevation_map;
uniform sampler2D precipitation_map;
uniform sampler2D humidity_map;

// Biome Colors Representation
uniform vec3 ocean_color = vec3(0.0, 0.3, 0.6);            // Deep blue
uniform vec3 lake_color = vec3(0.2, 0.5, 0.8);             // Light blue
uniform vec3 desert_color = vec3(0.9, 0.8, 0.4);           // Sandy yellow
uniform vec3 grassland_color = vec3(0.3, 0.7, 0.2);        // Green
uniform vec3 savanna_color = vec3(0.8, 0.7, 0.3);          // Golden tan
uniform vec3 tundra_color = vec3(0.7, 0.7, 0.8);           // Icy gray-blue
uniform vec3 boreal_forest_color = vec3(0.1, 0.4, 0.2);    // Dark green
uniform vec3 temperate_forest_color = vec3(0.2, 0.5, 0.1); // Forest green
uniform vec3 rainforest_color = vec3(0.0, 0.4, 0.0);       // Deep jungle green
uniform vec3 mountain_color = vec3(0.6, 0.6, 0.6);         // Gray stone

// Biome Thresholds

// OCEAN THRESHOLDS
uniform float ocean_e_min = 0.0;
uniform float ocean_e_max = 0.2;
// uniform float ocean_t_min = 0.0;
// uniform float ocean_t_max = 1.0;
uniform float ocean_p_min = 0.0;
uniform float ocean_p_max = 1.0;
uniform float ocean_h_min = 0.0;
uniform float ocean_h_max = 1.0;

// LAKE THRESHOLDS
uniform float lake_e_min = 0.0;
uniform float lake_e_max = 0.2;
// uniform float lake_t_min = 0.0;
// uniform float lake_t_max = 1.0;
uniform float lake_p_min = 0.7;
uniform float lake_p_max = 1.0;
uniform float lake_h_min = 0.7;
uniform float lake_h_max = 1.0;

// DESERT THRESHOLDS
uniform float desert_e_min = 0.2;
uniform float desert_e_max = 0.8;
// uniform float desert_t_min = 0.6;
// uniform float desert_t_max = 1.0;
uniform float desert_p_min = 0.0;
uniform float desert_p_max = 0.3;
uniform float desert_h_min = 0.0;
uniform float desert_h_max = 0.3;

// GRASSLAND THRESHOLDS
uniform float grassland_e_min = 0.2;
uniform float grassland_e_max = 0.7;
// uniform float grassland_t_min = 0.3;
// uniform float grassland_t_max = 0.6;
uniform float grassland_p_min = 0.3;
uniform float grassland_p_max = 0.6;
uniform float grassland_h_min = 0.3;
uniform float grassland_h_max = 0.6;

// SAVANNA THRESHOLDS
uniform float savanna_e_min = 0.2;
uniform float savanna_e_max = 0.5;
// uniform float savanna_t_min = 0.6;
// uniform float savanna_t_max = 1.0;
uniform float savanna_p_min = 0.3;
uniform float savanna_p_max = 0.6;
uniform float savanna_h_min = 0.3;
uniform float savanna_h_max = 0.6;

// TUNDRA THRESHOLDS
uniform float tundra_e_min = 0.2;
uniform float tundra_e_max = 0.7;
// uniform float tundra_t_min = 0.0;
// uniform float tundra_t_max = 0.3;
uniform float tundra_p_min = 0.0;
uniform float tundra_p_max = 0.4;
uniform float tundra_h_min = 0.2;
uniform float tundra_h_max = 0.4;

// BOREAL FOREST THRESHOLDS
uniform float boreal_forest_e_min = 0.3;
uniform float boreal_forest_e_max = 0.8;
// uniform float boreal_forest_t_min = 0.0;
// uniform float boreal_forest_t_max = 0.4;
uniform float boreal_forest_p_min = 0.4;
uniform float boreal_forest_p_max = 0.7;
uniform float boreal_forest_h_min = 0.4;
uniform float boreal_forest_h_max = 0.7;

// TEMPERATE FOREST THRESHOLDS
uniform float temperate_forest_e_min = 0.2;
uniform float temperate_forest_e_max = 0.6;
// uniform float temperate_forest_t_min = 0.3;
// uniform float temperate_forest_t_max = 0.6;
uniform float temperate_forest_p_min = 0.5;
uniform float temperate_forest_p_max = 0.8;
uniform float temperate_forest_h_min = 0.5;
uniform float temperate_forest_h_max = 0.8;

// RAINFOREST THRESHOLDS
uniform float rainforest_e_min = 0.2;
uniform float rainforest_e_max = 0.5;
// uniform float rainforest_t_min = 0.7;
// uniform float rainforest_t_max = 1.0;
uniform float rainforest_p_min = 0.8;
uniform float rainforest_p_max = 1.0;
uniform float rainforest_h_min = 0.8;
uniform float rainforest_h_max = 1.0;

// MOUNTAIN THRESHOLDS
uniform float mountain_e_min = 0.7;
uniform float mountain_e_max = 1.0;
// uniform float mountain_t_min = 0.0;
// uniform float mountain_t_max = 0.4;
uniform float mountain_p_min = 0.0;
uniform float mountain_p_max = 1.0;
uniform float mountain_h_min = 0.0;
uniform float mountain_h_max = 1.0;

uniform float blend_radius = 0.2; // Used to smoothly blend biomes together

void fragment(){
	float elevation = texture(elevation_map, UV).r;
	float precipitation = texture(precipitation_map, UV).r;
	float humidity = texture(humidity_map, UV).r;
	
	vec3 colors[10] = vec3[10](
		ocean_color, lake_color, desert_color, grassland_color, savanna_color, 
		tundra_color, boreal_forest_color, temperate_forest_color,
		rainforest_color, mountain_color
	);
	
	float min_e[10] = float[10](
		ocean_e_min, lake_e_min, desert_e_min, grassland_e_min, savanna_e_min, 
		tundra_e_min, boreal_forest_e_min, temperate_forest_e_min,
		rainforest_e_min, mountain_e_min
	);
	
	float max_e[10] = float[10](
		ocean_e_max, lake_e_max, desert_e_max, grassland_e_max, savanna_e_max, 
		tundra_e_max, boreal_forest_e_max, temperate_forest_e_max,
		rainforest_e_max, mountain_e_max
	);
	
	float min_p[10] = float[10](
		ocean_p_min, lake_p_min, desert_p_min, grassland_p_min, savanna_p_min, 
		tundra_p_min, boreal_forest_p_min, temperate_forest_p_min,
		rainforest_p_min, mountain_p_min
	);
	
	float max_p[10] = float[10](
		ocean_p_max, lake_p_max, desert_p_max, grassland_p_max, savanna_p_max, 
		tundra_p_max, boreal_forest_p_max, temperate_forest_p_max,
		rainforest_p_max, mountain_p_max
	);
	
	float min_h[10] = float[10](
		ocean_h_min, lake_h_min, desert_h_min, grassland_h_min, savanna_h_min, 
		tundra_h_min, boreal_forest_h_min, temperate_forest_h_min,
		rainforest_h_min, mountain_h_min
	);
	
	float max_h[10] = float[10](
		ocean_h_max, lake_h_max, desert_h_max, grassland_h_max, savanna_h_max, 
		tundra_h_max, boreal_forest_h_max, temperate_forest_h_max,
		rainforest_h_max, mountain_h_max
	);
	
	float weights[10];
	
	// NOTE: blend radius formula causes some murky/overlapping transitions, replace with better formula if possible
		for (int i = 0; i < 10; i++){
		float e_weight = 0.0;
		float p_weight = 0.0;
		float h_weight = 0.0;
		
		if (precipitation < min_p[i] - blend_radius || precipitation > max_p[i] + blend_radius || humidity < min_h[i] - blend_radius || humidity > max_h[i] + blend_radius){
			weights[i] = 0.0;
		} else {
			if (elevation < min_e[i]){
				e_weight = smoothstep(min_e[i] - blend_radius, min_e[i], elevation);
			} else if (elevation > max_e[i]){
				e_weight = smoothstep(max_e[i] + blend_radius, max_e[i], elevation);
			} else {
				e_weight = 1.0;
			}
			
			if (precipitation < min_p[i]){
				p_weight = smoothstep(min_p[i] - blend_radius, min_p[i], precipitation);
			} else if (precipitation > max_p[i]){
				p_weight = smoothstep(max_p[i] + blend_radius, max_p[i], precipitation);
			} else {
				p_weight = 1.0;
			}
			
			if (humidity < min_h[i]){
				h_weight = smoothstep(min_h[i] - blend_radius, min_h[i], humidity);
			} else if (humidity > max_h[i]){
				h_weight = smoothstep(max_h[i] + blend_radius, max_h[i], humidity);
			} else {
				h_weight = 1.0;
			}
			
			weights[i] = (e_weight + p_weight + h_weight) / 3.0; 
		}
		
		// Normalize weights to a sum of 1.0
		float total_weight = 0.0;
		for (int i = 0; i < 10; i++){
			total_weight += weights[i];
		}
		if (total_weight > 0.0){
			for (int i = 0; i < 10; i++){
				weights[i] /= total_weight;
			} 
		}
		
		vec3 blended_color = vec3(0.0);
		for (int i = 0; i < 10; i++){
			blended_color += colors[i] * weights[i];
		}
		
		COLOR.rgb = blended_color;
	}
}