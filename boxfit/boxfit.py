import numpy as np
import itertools
def boxfit(container_dims, packages):
    fill_mask = np.zeros(container_dims)
    used = []
    vol = 0
    for i,package in enumerate(packages):
        for pos in itertools.product(range(0,container_dims[0]),
                                     range(0,container_dims[1]),
                                     range(0,container_dims[2])):
            #Check if the package fits in the position
            if pos[0] + package[0] >= container_dims[0]:
                continue
            if pos[1] + package[1] >= container_dims[1]:
                continue
            if pos[2] + package[2] >= container_dims[2]:
                continue

            if np.sum(fill_mask[pos[0]:pos[0]+package[0],
                                pos[1]:pos[1]+package[1],
                                pos[2]:pos[2]+package[2]]) > 0:
                continue
            
            fill_mask[pos[0]:pos[0]+package[0],
                      pos[1]:pos[1]+package[1],
                      pos[2]:pos[2]+package[2]] = 1
            used.append(i)
            vol = vol + package[0]*package[1]*package[2]
            break
            

    return used,(1.0*vol)/(container_dims[0]*container_dims[1]*container_dims[2])

container_dims = [25, 56, 18]

packages = [[1, 1, 1],
            [8, 39, 4],
            [8, 39, 4],
            [8, 52, 4],
            [24, 23, 14],
            [12, 12, 7],
            [16, 21, 11],
            [15, 29, 10],
            [24, 36, 10],
            [21, 39, 7]]

print(boxfit(container_dims, packages))