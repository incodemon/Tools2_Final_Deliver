-- Read any kind of animation 
-- Write the animation keyframes into a file with the format:
-- First line: entity name
-- Second line: frames per second
-- Animation frame composed by:

global outputDirectory = "../data/assets/"
global MaxToMVD = rotateXMatrix -90

struct MVDAnimationFrame
(
	objPosition,
	objRotation,
	objScale
)

-- Retrieve the frames from the animation
fn fetchFrames obj nframes = 
(
	frames = #()
	
	-- Loop through each of the keyframes
	with animate on
	for i = 0 to nframes do 
	(
		at time i 
		(
			local cframe = MVDAnimationFrame()
			cframe.objPosition = obj.position
			cframe.objRotation = obj.rotation
			cframe.objScale = obj.scale		
			append frames cframe
		)
	)
	
	return frames
)

-- Method to export the animation file
fn exportAnimation filename obj nframes = 
(
	-- Read the frames
	frames = fetchFrames obj nframes
	
	-- Write the file with the frame data
	f = createFile (outputDirectory + filename +".anim")
	format (obj.name + "\n") to:f
	format ((nframes as string) + "\n") to:f
	
	-- TO-DO, SUPPORT MORE THAN ONE OBJECT PER FILE (N OBJECTS)
	-- Animate a door 
	
	for i = 1 to nframes do 
	(
		local cframe = frames[i]
		local mvd_pos = cframe.objPosition * MaxToMVD
		local tmp_rot =  (cframe.objRotation as eulerAngles) 
		local mvd_rot = [tmp_rot.x, tmp_rot.y, tmp_rot.z] * MaxToMVD
		local mvd_scale = [cframe.objScale.x, cframe.objScale.z, cframe.objScale.y]
		
		local str_position = (mvd_pos.x as string) + " " + (mvd_pos.y as string) + " " + (mvd_pos.z as string)
		local str_rotation = (mvd_rot.x as string) + " " + (mvd_rot.y as string) + " " + (mvd_rot.z as string)
		local str_scale = (mvd_scale.x as string) + " " + (mvd_scale.y as string) + " " + (mvd_scale.z as string)
		local str_final = (i - 1) as string + " " + str_position + " " + str_rotation + " " + str_scale + "\n"
		print str_final
		format str_final to:f
	)
	
	close f
)

exportAnimation "bounce4" $ 55