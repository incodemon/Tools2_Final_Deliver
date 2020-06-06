#export camera and target

import maya.cmds as mc

#export file
myFileObject=open('/Users/alun/Dropbox/Work/LaSalle/teaching/master_game_dev/code/20-BasicAnimation/data/assets/camera_aim.anim', 'w')

#get time and attributes
minTime = mc.playbackOptions(query=True, minTime=True)
maxTime = mc.playbackOptions(query=True, maxTime=True)
attributes = ['rotateX', 'rotateY', 'rotateZ']

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
        
        camPos = cmds.xform(obs[0], q=True, t=True, ws=True)
        for cp in camPos:
            myAt = "{0:.5f}".format(cp)
            currentObjectData.append(myAt + ' ')
        
        for theAttribute in attributes:
            myAtF = mc.getAttr(selection + '.' + theAttribute)
            myAt = myAt = "{0:.5f}".format(myAtF)
            currentObjectData.append(myAt + ' ')
        
        centerOfInterest = mc.camera(obs[0], q=True, worldCenterOfInterest=True)
        
        for coi in centerOfInterest:
            myAt = "{0:.5f}".format(coi)
            currentObjectData.append(myAt + ' ')
        
        currentObjectData.append('\n')   
    theData[name] = currentObjectData

for objects in theData:
    myFileObject.writelines(objects + '\n')
    myFileObject.writelines('24\n');
    for lines in theData[objects]:
        myFileObject.writelines(lines)
myFileObject.close()