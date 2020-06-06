import maya.cmds as mc

#export file
myFileObject=open('/Users/alun/Dropbox/Work/Code/Projects/OpenGL_Book/assets/maya_project/scenes/data.txt', 'w')

#get time and attributes
minTime = mc.playbackOptions(query=True, minTime=True)
maxTime = mc.playbackOptions(query=True, maxTime=True)
attributes = ['translateX', 'translateY', 'translateZ', 'rotateX', 'rotateY', 'rotateZ', 'scaleX', 'scaleY', 'scaleZ']

#dictionary for all data,
theData = {}

#Create, in dictionary, a list for each object in selection, referenced by object name
obs = mc.ls(sl=True)
count = 0
for selection in obs:
    name = obs[count]
    count +=1
    theData[name] = []

#for each keyframe
for time in range(int(minTime -1), int(maxTime +1)):
    mc.currentTime(time)
    count = 0
    print time
    for selection in obs:
        name = obs[count]
        count +=1
        currentObjectData = theData[name]
        myTime = str(time)
        currentObjectData.append(myTime + ' ')
        for theAttribute in attributes:
            myAtF = mc.getAttr(selection + '.' + theAttribute)
            myAt = str(myAtF)
            currentObjectData.append(myAt + ' ')
        
        currentObjectData.append('\n')   
    theData[name] = currentObjectData

for objects in theData:
    myFileObject.writelines(objects + '\n')
    for lines in theData[objects]:
        myFileObject.writelines(lines)
myFileObject.close()