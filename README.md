# WayFinderOSM
A software which reads data about various artifacts in the OSM file, and provides a text searchable interface to the map.
It uses RapidXml to parse the OSM file, calculates edge weights using the haversine formula and uses STL to apply Dijkstra's Algorithm while finding shortest paths.

>> Run 20CS30058.cpp to parse and read map.osm
