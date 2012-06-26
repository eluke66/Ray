# Window gaps: 0 = no gap, 1.0 = window width
# hRepeat = windows per side horizontally
# vRepeat = windows from bottom to top (stories)
def makeBuildingTexture( hGap,
                         vGap,
                         windowMaterial,
                         nonWindowMaterial,
                         hRepeat=10,
                         vRepeat=50,
                         mapSize=10):

    # Create a material that modulates between the window and
    # nonWindow materials. We do this by creating an image of size
    # mapSize and setting the pixels programmatically.
    texMap = Image( mapSize, mapSize )

    xChange = mapSize * hGap
    yChange = mapSize * vGap
    for x in range(0, mapSize-1):
        for y in range(0, mapSize-1):
            if (x >= xChange and y >= yChange ):
                texMap.set( x, y, 1.0 )
            else:
                texMap.set( x, y, 0.0 )
                
    # Now create a modulating texture
    
        
    
