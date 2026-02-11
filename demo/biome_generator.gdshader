shader_type canvas_item;

uniform sampler2D elevation_map;
uniform sampler2D temperature_map;
uniform sampler2D precipitation_map;
uniform sampler2D humidity_map;

// Biome Colors Representation - High Contrast Version
uniform vec3 ocean_color = vec3(0.02, 0.15, 0.45);            // Rich Navy 
uniform vec3 lake_color = vec3(0.0, 0.6, 0.9);                // Vibrant Cyan 
uniform vec3 desert_color = vec3(1.0, 0.85, 0.3);             // Bright Gold 
uniform vec3 grassland_color = vec3(0.4, 0.85, 0.1);          // Lime Green
uniform vec3 savanna_color = vec3(0.8, 0.6, 0.2);             // Burnt Orange/Ochre 
uniform vec3 tundra_color = vec3(0.85, 0.9, 1.0);             // Bright Arctic White-Blue
uniform vec3 boreal_forest_color = vec3(0.05, 0.25, 0.15);    // Deep Pine Green 
uniform vec3 temperate_forest_color = vec3(0.1, 0.55, 0.05);  // Mid Grass Green
uniform vec3 rainforest_color = vec3(0.0, 0.35, 0.05);        // Lush Emerald 
uniform vec3 mountain_color = vec3(0.4, 0.35, 0.3);           // Warm Slate 

// Biome Thresholds

// OCEAN: Lowered max elevation slightly, kept full range for others to catch all low-lands
uniform float ocean_e_min = -0.1; // Extended below 0 for safety
uniform float ocean_e_max = 0.25;
uniform float ocean_t_min = 0.0;
uniform float ocean_t_max = 1.0;
uniform float ocean_p_min = 0.0;
uniform float ocean_p_max = 1.0;
uniform float ocean_h_min = 0.0;
uniform float ocean_h_max = 1.0;

// LAKE: High humidity/precip low-lands
uniform float lake_e_min = 0.0;
uniform float lake_e_max = 0.25;
uniform float lake_t_min = 0.0;
uniform float lake_t_max = 1.0;
uniform float lake_p_min = 0.65; // Overlap with Ocean/Forests
uniform float lake_p_max = 1.0;
uniform float lake_h_min = 0.65;
uniform float lake_h_max = 1.0;

// TUNDRA: Cold and Dry
uniform float tundra_e_min = 0.2;
uniform float tundra_e_max = 0.75;
uniform float tundra_t_min = 0.0;
uniform float tundra_t_max = 0.35; // Meets Grassland/Boreal
uniform float tundra_p_min = 0.0;
uniform float tundra_p_max = 0.45;
uniform float tundra_h_min = 0.0;
uniform float tundra_h_max = 0.45;

// BOREAL FOREST: Cold and Moist
uniform float boreal_forest_e_min = 0.2;
uniform float boreal_forest_e_max = 0.8;
uniform float boreal_forest_t_min = 0.0;
uniform float boreal_forest_t_max = 0.45;
uniform float boreal_forest_p_min = 0.4;
uniform float boreal_forest_p_max = 0.75;
uniform float boreal_forest_h_min = 0.4;
uniform float boreal_forest_h_max = 0.75;

// GRASSLAND: Temperate and Mid-Moisture
uniform float grassland_e_min = 0.2;
uniform float grassland_e_max = 0.75;
uniform float grassland_t_min = 0.3;
uniform float grassland_t_max = 0.65;
uniform float grassland_p_min = 0.25;
uniform float grassland_p_max = 0.65;
uniform float grassland_h_min = 0.25;
uniform float grassland_h_max = 0.65;

// TEMPERATE FOREST: Temperate and Humid
uniform float temperate_forest_e_min = 0.2;
uniform float temperate_forest_e_max = 0.65;
uniform float temperate_forest_t_min = 0.3;
uniform float temperate_forest_t_max = 0.75;
uniform float temperate_forest_p_min = 0.6;
uniform float temperate_forest_p_max = 0.85;
uniform float temperate_forest_h_min = 0.6;
uniform float temperate_forest_h_max = 0.85;

// DESERT: Hot and Dry
uniform float desert_e_min = 0.2;
uniform float desert_e_max = 0.85;
uniform float desert_t_min = 0.6;
uniform float desert_t_max = 1.1; // Extended to catch peaks
uniform float desert_p_min = 0.0;
uniform float desert_p_max = 0.35;
uniform float desert_h_min = 0.0;
uniform float desert_h_max = 0.35;

// SAVANNA: Hot and Mid-Moisture
uniform float savanna_e_min = 0.2;
uniform float savanna_e_max = 0.6;
uniform float savanna_t_min = 0.6;
uniform float savanna_t_max = 1.1;
uniform float savanna_p_min = 0.3;
uniform float savanna_p_max = 0.7;
uniform float savanna_h_min = 0.3;
uniform float savanna_h_max = 0.7;

// RAINFOREST: Hot and Very Wet
uniform float rainforest_e_min = 0.2;
uniform float rainforest_e_max = 0.55;
uniform float rainforest_t_min = 0.65;
uniform float rainforest_t_max = 1.1;
uniform float rainforest_p_min = 0.65;
uniform float rainforest_p_max = 1.1;
uniform float rainforest_h_min = 0.65;
uniform float rainforest_h_max = 1.1;

// MOUNTAIN: High Elevation (Catch-all for anything above 0.7)
uniform float mountain_e_min = 0.7;
uniform float mountain_e_max = 1.2;
uniform float mountain_t_min = 0.0;
uniform float mountain_t_max = 1.0;
uniform float mountain_p_min = 0.0;
uniform float mountain_p_max = 1.0;
uniform float mountain_h_min = 0.0;
uniform float mountain_h_max = 1.0;

uniform float blend_radius = 0.05; // Used to smoothly blend biomes together 

void fragment(){
	float elevation = texture(elevation_map, UV).r;
	float temperature = texture(temperature_map, UV).r;
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
	
	float min_t[10] = float[10](
		ocean_t_min, lake_t_min, desert_t_min, grassland_t_min, savanna_t_min, 
		tundra_t_min, boreal_forest_t_min, temperate_forest_t_min,
		rainforest_t_min, mountain_t_min
	);
	
	float max_t[10] = float[10](
		ocean_t_max, lake_t_max, desert_t_max, grassland_t_max, savanna_t_max, 
		tundra_t_max, boreal_forest_t_max, temperate_forest_t_max,
		rainforest_t_max, mountain_t_max
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
	// HOWEVER, areas in these transitions ensure that areas are not evenly weighted with a mashup of multiple biomes
	for (int i = 0; i < 10; i++){
		float e_weight = 0.0;
		float t_weight = 0.0;
		float p_weight = 0.0;
		float h_weight = 0.0;
		
		// If all four conditions are active, the biome shader map can have black patches due to NO biome-types falling within those thresholds
		// This is the most conditions we can fit without patching
		if (
			// elevation < min_e[i] - blend_radius || elevation > max_e[i] + blend_radius ||        
			temperature < min_t[i] - blend_radius || temperature > max_t[i] + blend_radius ||
			precipitation < min_p[i] - blend_radius || precipitation > max_p[i] + blend_radius ||
			humidity < min_h[i] - blend_radius || humidity > max_h[i] + blend_radius
		){
			weights[i] = 0.0;
		} else {
			if (elevation < min_e[i]){
				e_weight = smoothstep(min_e[i] - blend_radius, min_e[i], elevation);
			} else if (elevation > max_e[i]){
				e_weight = smoothstep(max_e[i] + blend_radius, max_e[i], elevation);
			} else {
				e_weight = 1.0;
			}
			
			if (temperature < min_t[i]){
				t_weight = smoothstep(min_t[i] - blend_radius, min_t[i], temperature);
			} else if (temperature > max_t[i]){
				t_weight = smoothstep(max_t[i] + blend_radius, max_t[i], temperature);
			} else {
				t_weight = 1.0;
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
			
			weights[i] = (e_weight + t_weight + p_weight + h_weight) / 4.0; 
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