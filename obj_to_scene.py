import sys

obj = open(sys.argv[1])
vertices = []
normals = []
triangles = []

dummy_vertex = [0,0,0]
try:
    for line in obj:
        line = line.strip()
        if len(line) == 0:
            continue
        if line[0] == '#':
            continue

        pieces = line.split()
        if pieces[0] == 'v':
            vertices.append(pieces[1:])
        elif pieces[0] == 'vn':
            normals.append(pieces[1:])
        elif pieces[0] == 'f':
            coords = [num.partition('/')[0] for num in pieces[1:]]
            triangles.append(coords[:3])
            if len(coords) == 4: # We have a quad
                triangles.append([coords[0], coords[2], coords[3]])
        else:
            continue

finally:
    obj.close()

num_vertices = len(vertices)
print "maxvertnorms", num_vertices + 1

print "vertexnormal 0 0 0 0 0 0"

for i in xrange(num_vertices):
    vertex = vertices[i]
    normal = normals[i]
    print "vertexnormal", vertex[0], vertex[1], vertex[2], normal[0], normal[1], normal[2]
    
for triangle in triangles:
    print "trinormal", triangle[0], triangle[1], triangle[2]


