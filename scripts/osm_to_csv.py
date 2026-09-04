import json
import urllib.request
import math
import sys

# Bounding box for Kharagpur (approx: 22.31 to 22.36 Lat, 87.29 to 87.35 Lon)
overpass_url = "http://overpass-api.de/api/interpreter"
overpass_query = """
[out:json][timeout:25];
(
  way["highway"](22.3100, 87.2900, 22.3600, 87.3500);
);
out body;
>;
out skel qt;
"""

print("Fetching OpenStreetMap data for Kharagpur, West Bengal...")
req = urllib.request.Request(overpass_url, data=overpass_query.encode('utf-8'), headers={'User-Agent': 'CityTrafficRouting/1.0'})
try:
    response = urllib.request.urlopen(req)
    data = json.loads(response.read().decode('utf-8'))
except Exception as e:
    print(f"Failed to fetch data: {e}")
    sys.exit(1)

nodes = {}
ways = []

print("Parsing OSM JSON data...")
for element in data['elements']:
    if element['type'] == 'node':
        nodes[element['id']] = (element['lat'], element['lon'])
    elif element['type'] == 'way':
        if 'nodes' in element:
            ways.append(element['nodes'])

if not nodes:
    print("No nodes found.")
    sys.exit(1)

ref_lat, ref_lon = list(nodes.values())[0]

def latlon_to_xy(lat, lon):
    # Equirectangular approximation to project to Cartesian meters
    r_earth = 6371000.0
    x = r_earth * math.radians(lon - ref_lon) * math.cos(math.radians(ref_lat))
    y = r_earth * math.radians(lat - ref_lat)
    return x, y

def distance(x1, y1, x2, y2):
    return math.hypot(x2 - x1, y2 - y1)

# C++ engine expects 32-bit integers, OSM uses 64-bit. We remap IDs to 0, 1, 2...
node_id_map = {}
next_id = 0

out_nodes = []
out_edges = []

for way_nodes in ways:
    for i in range(len(way_nodes) - 1):
        n1 = way_nodes[i]
        n2 = way_nodes[i+1]
        
        if n1 not in nodes or n2 not in nodes:
            continue
            
        if n1 not in node_id_map:
            node_id_map[n1] = next_id
            next_id += 1
            x, y = latlon_to_xy(*nodes[n1])
            out_nodes.append((node_id_map[n1], x, y))
            
        if n2 not in node_id_map:
            node_id_map[n2] = next_id
            next_id += 1
            x, y = latlon_to_xy(*nodes[n2])
            out_nodes.append((node_id_map[n2], x, y))
            
        id1 = node_id_map[n1]
        id2 = node_id_map[n2]
        
        x1, y1 = latlon_to_xy(*nodes[n1])
        x2, y2 = latlon_to_xy(*nodes[n2])
        dist = distance(x1, y1, x2, y2)
        
        # Add bidirectional edges as requested
        out_edges.append((id1, id2, dist))
        out_edges.append((id2, id1, dist))

print(f"Writing {len(out_nodes)} unique nodes and {len(out_edges)} edges to CSV...")
with open("data/osm_graph.csv", "w") as f:
    for nid, x, y in out_nodes:
        f.write(f"NODE,{nid},OSM_{nid},{x:.2f},{y:.2f}\n")
    for src, dst, dist in out_edges:
        f.write(f"EDGE,{src},{dst},{dist:.2f}\n")
        
print("Success! Generated data/osm_graph.csv")
