extends Node2D

@onready var viewport = get_node('SubViewport')
@onready var timer = get_node('Timer')

var wc = WorldConfiguration.new()
var texture

func _ready():
	viewport.size = Vector2(wc.size, wc.size)
	texture = viewport.get_texture()
	
	timer.timeout.connect(generate_humidity_map.bind())
	timer.start()
	
func generate_humidity_map():
	var image = texture.get_image()
	var image_texture = ImageTexture.create_from_image(image)
	
	# Save texture in this path 
	var path = "res://Images/HumidityMap.tres"
	ResourceSaver.save(image_texture, path)
	print("Saved Humidity Map Image")
