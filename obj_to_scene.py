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
            triangles.append([num.partition('/')[0] for num in pieces[1:]])
        #triangles.append([pieces[1].partition('/')[0],
        #                  pieces[2].partition('/')[1],
        #                  pieces[3].partition('/')[2]])
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


