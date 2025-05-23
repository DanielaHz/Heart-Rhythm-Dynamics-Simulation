import gmsh
import json

# Created to access quickly to the API gmsh for python (c++ implementation and constuction projects took 1 hour)
input_file = "Game/resources/3D/fun/newHeart-test03.msh"
output_file = "Game/resources/3D/fun/newHeart-test03.msh"

gmsh.initialize()
gmsh.open(input_file)

# Get node
node_tags, node_coords, _ = gmsh.model.mesh.getNodes()

nodes = []
for i in range(0, len(node_coords), 3):
    x = float(node_coords[i])
    y = float(node_coords[i + 1])
    z = float(node_coords[i + 2])
    nodes.append([x, y, z])

# Get tetrahedros elements (tag 4)
elements = []
types, tags, node_ids = gmsh.model.mesh.getElements()

for elem_type, elem_tags, elem_nodes in zip(types, tags, node_ids):
    if elem_type == 4:  # Tetrahedros
        for i in range(0, len(elem_nodes), 4):
            t = [
                int(elem_nodes[i]) - 1,
                int(elem_nodes[i + 1]) - 1,
                int(elem_nodes[i + 2]) - 1,
                int(elem_nodes[i + 3]) - 1
            ]
            elements.append(t)

gmsh.finalize()

# Saved this file as json 
data = {
    "nodes": nodes,
    "tetrahedra": elements
}

with open(output_file, "w") as f:
    json.dump(data, f, indent=2)
