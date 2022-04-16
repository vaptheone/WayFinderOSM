/*
SOFTWARE ENGINEERING LABORATORY
Assignment: 2
Name: Vineet Amol Pippal
Roll Number: 20CS30058
*/
#include <bits/stdc++.h>
#include "rapidxml.hpp"
#include "rapidxml_utils.hpp"
#include <iostream>
#include <cstdlib>
#include <cassert>
#include <new>
#include <exception>

using namespace rapidxml;
using namespace std;

xml_document<> D;
xml_node<> *n = nullptr;
double MAX = DBL_MAX;

//for sorting a vector of pairs in ascending order, by the second argument
bool sortbysec(const pair<int, double> &a, const pair<int, double> &b)
{return (a.second < b.second);}

typedef pair <double, int> elem;

typedef struct Node//Node struct to define a node
{
    string id;
    double lat,lon; //latitude and longitude
    string name="";//name will be empty unless given in tag
}Node;

typedef struct Way//Way struct to define a way
{
    string id;
    vector <Node> N;//a vector of Nodes referenced to in each way tag of osm file
    string name="";
}Way;

//Haversine formula to find real-world distance between two nodes using their longitudes and latitudes
double haversine(Node a, Node b) 
{
    double R = 6371e3 ,phi1 = a.lat * M_PI/180.0, phi2 = b.lat * M_PI/180.0;
    double delPhi = (a.lat - b.lat) * M_PI/180.0, delLambda = (a.lon - b.lon) * M_PI/180.0;
    double A = pow ( sin( delPhi/2 ) , 2) + cos(phi1) * cos(phi2) * pow( sin(delLambda/2) , 2 );
    double c = 2 * asin (sqrt(A));
    return R * c / 1000;//returning distance in kilometres
}

//to check whether b is a substring of a
bool isSubstring(string a, string b)
{
    size_t found = a.find(b);
    if (found != string::npos) return true;
    return false;
}

//searching for a node
void search_Node(vector <Node> v, string s)
{
    int cnt=0, i;
    //strlwr function to make searching independent of case (case insensitive)
    s = strlwr (&s[0]);

    for (i=0; i<v.size(); ++i)
    {
        string b = v[i].name;
        b = strlwr(&b[0]);
        if ( isSubstring (b, s) )
        {
            cout<<"Node id: "<<v[i].id<<", latitude: "<<v[i].lat<<", longitude: "<<v[i].lon;
            if (v[i].name != "") cout<<" name: "<<v[i].name;
            cout<<endl;
            ++cnt;
        }
    }

    if (cnt == 0) 
    {
        cout<<"No nodes found with given substring names!\n";
        return;
    }
    cout<<cnt<<" result nodes found.\n";
}

//searching for a way node
void search_Way(vector <Way> w,string s)
{
    int cnt=0, i;
    s = strlwr(&s[0]);
    for (i=0; i<w.size(); ++i)
    {
        string p = w[i].name;
        p = strlwr( &p[0] );
        if (isSubstring (p , s))
        cout<<"Way Name: "<<w[i].name<<", id: "<<w[i].id<<endl, ++cnt;
    }
    
    if (cnt == 0) 
    {
        cout<<"No ways found with given substring names!\n";
        return;
    }
    cout<<cnt<<" result ways found.\n";
}

//function to find k closest nodes to a node
void k_closest(vector <Node> v, string ID, int k)
{
    vector <pair <int, double>> dist; 

    int i,j;
    for (i=0; i< v.size(); ++i)
    if (v[i].id == ID) break;

    if (i== v.size()) 
    {
        cout<<"Invalid input!\n";
        return;
    }
    else 
    for (j=0; j<v.size(); ++j)
    {
        pair <int, double> p;
        p.first = j;
        p.second = haversine (v[i], v[j]);
        dist.push_back(p);
    }

    sort(dist.begin(), dist.end(), sortbysec);
    cout<<k<<" nearest node to given node are:- \n";
    for (i=1; i<=k; ++i)
    {
        j = dist[i].first;
        cout<<"Node id: "<<v[j].id<<", latitude: "<<v[j].lat<<", longitude: "<<v[j].lon;
        if (v[j].name != "") cout<<" name: "<<v[j].name; 
        cout<<", distance = "<<dist[i].second<<" km.\n";
    }
}

//Implementation of Dijkstra's Algorithm to find the shortest path between 2 nodes
//(h is the map of string id to its corresponding node: v is node vector: w is way vector: id1 and id2 are node ids between which, the shortest path needs to be found) 
void shortest_path(map <string, Node> h, vector <Node> v, vector<Way> w, string id1, string id2)
{
    //mapping string id's to sets of string id's : representing an adjacency list
    //we can, for a given node id, directly check whether a particular node (using its id) is directly connected to it
    map <string, set <string>> m;
    //mapping string id's to their corresponding indexes in v and w: to provide direct access
    map <string, int> r;
    int i, j ,Vnear;
    for (i =0; i<v.size(); ++i)
    {
        set <string> s; //inserting an empty set
        m.insert({v[i].id, s});
        r.insert({v[i].id, i});
    }

    for (i=0; i< w.size(); ++i )
    {
        for (j=0; j<w[i].N.size()-1; ++j)
        {
            m[w[i].N[j].id].insert(w[i].N[j+1].id);
            m[w[i].N[j+1].id].insert(w[i].N[j].id);
        }
    }

    for (j=0; j<v.size (); ++j)
    if (v[j].id == id1) break;//j is id of start node (corresponding to id1)
    
    //visited vector to keep a track of visited nodes: dist vector to store and update distances from our start node (node 1)
    vector <bool> visited (v.size(), false);
    vector <double> dist (v.size(), MAX);
    vector <int> parent (v.size(),j);
    visited [j]= true;//node 1 already visited

    //priority queue to push traversable nodes inside, and to pop the node with least distance, in each iteration
    priority_queue <elem, vector<elem>, greater<elem>> q;

    for (i=0; i< v.size(); ++i)
    if ( m.at(id1).find( v[i].id ) != m.at(id1).end() ) 
    {
        //pushing and updatind dist for the nodes directly connected to start node
        dist[i] = haversine (v[i], h[id1]);
        q.push({dist[i], i});
    }
    dist[j] = 0;
    j=0;

    while (!q.empty())
    {
        pair <double, int> p = q.top();
        q.pop();

        //nearest vertex to be added in our MST is Vnear
        Vnear = p.second;
        visited[ Vnear ]= true;
        if (v[ Vnear ].id == id2 )//reached destination node
        {
            cout<<"Pathfinding completed... distance between the two nodes is "<< dist[ Vnear ] <<" kilometres."<<endl;
            //printing the shortest path between the two nodes
            vector <string> t;
            t.push_back(id2);
            while (t[t.size()-1]!=id1) t.push_back(v[parent[r[t[t.size()-1]]]].id);
            cout<<"Total nodes traversed on the path: "<<t.size()<<endl<<"Path taken:- "<<endl;

            for (i=0; i<t.size(); ++i) 
            {
                cout<<t[t.size()-i-1];
                if (i!= t.size()-1) cout<<"->";
            }
            cout<<endl;
            return;
        }

        //Relaxation: after adding Vnear to our MST, we update dist and push the nodes directly connected to it, into the priority queue
        for (auto i: m[v[Vnear].id])
        {
            j = r[i];
            if (!visited[j] && haversine (v[Vnear],v[j]) + dist[Vnear] < dist[j])
            {
                dist[j] = dist[Vnear] + haversine (v[Vnear], v[j]);
                q.push({dist[j], j});
                parent [j] = Vnear;
            }
        }
    }
    cout<<"No path possible. Exiting...\n";
}

int main()
{
    //opening the osm file and creating buffer 
    ifstream osm_file ("map.osm");
    vector<char> buffer ((istreambuf_iterator <char> (osm_file)) , istreambuf_iterator <char>());
    buffer.push_back('\0');

    //parsing the osm file 
    D.parse <0> (& buffer[0]);

    //node Vector and way Vector 
    vector <Node> NodeList;
    vector <Way> WayList;
    map <string, Node> NodeMap;

    //root node* points to the osm tag 
    xml_node<> * root = D.first_node("osm");
    n = root->first_node("node");

    cout<<std::setprecision(15)<<"Parsing file...\n";
    string line = "==="; 
    int i=0;
    while (i++ < 27) line+="===";

    ////PART 1 :-
    //taking and storing nodes data from the osm file
    while (n)
    {
        Node t;
        t.id = (string) n->first_attribute("id")->value();
        t.lat = stod (n->first_attribute("lat")->value());
        t.lon = stod (n->first_attribute("lon")->value());
        
        xml_node<>* temp = n->first_node("tag");
        while (temp) //extracting names from named node tags
        {
            if ((string)temp->first_attribute("k")->value() == (string)"name") 
            {
                t.name = temp->first_attribute("v")->value();
                break;
            }
            else temp = temp->next_sibling("tag");
        }
        NodeMap.insert( pair <string, Node> (t.id, t));
        NodeList.push_back(t);
        n = n->next_sibling("node");
    }
    cout<<"Number of nodes found in this document is: "<<NodeMap.size()<<endl;

    //taking and storing way data from the osm file
    n = root->first_node("way");
    while (n)
    {
        Way w;
        w.id = (string)n->first_attribute("id")->value();

        xml_node<>* temp = n->first_node("nd");
        while (temp)
        {
            w.N.push_back( NodeMap[(string)temp->first_attribute("ref")->value()] );
            temp = temp->next_sibling("nd");
        }

        temp = n->first_node("tag");
        while (temp)
        {
            if ((string) temp->first_attribute("k")->value() == (string)"name")
            {
                w.name = temp->first_attribute("v")->value();
                break;
            }
            else temp = temp->next_sibling("tag");
        }

        WayList.push_back(w);
        n = n->next_sibling("way");
    }
    cout<<"Number of ways found in this document is: "<< WayList.size()<<endl;

    bool run= true;
    while (run)
    {
        cout<<"\nWhat would you like to?\nPress 1-> Search for an element\n"
        <<"Press 2-> Find k closest nodes to a particular node\nPress 3-> Calculate shortest path between 2 node elements\nInput: ";
        int x; cin>>x;
        switch (x)
        {
            //For searching elements (Nodes or Ways)
            case 1:
            {
                cout<<"Which element would you like to search for?\n1) Node\n2) Way\nInput: ";
                int y; cin>>y;

                if (y == 1)
                {
                    string s;
                    cout<<"Enter keyword substring: "; cin>>s;
                    search_Node(NodeList , s);
                }
                if (y == 2)
                {
                    string s;
                    cout<<"Enter keyword substring: "; cin>>s;
                    search_Way(WayList, s);
                }
                cout<<line<<endl;
                break;
            }

            //For k closest nodes to a particular node
            case 2:
            {
                cout<<"Enter target node id: "; 
                string ID; 
                cin>>ID;
                cout<<"Enter value of k: "; int k; cin>>k;
                k_closest(NodeList, ID , k);
                cout<<line<<endl;
                break;
            }

            //For shortest path between two nodes
            case 3:
            {
                string n1, n2;
                cout<<"Give id for node 1: "; cin>>n1;
                cout<<"Give id for node 2: "; cin>>n2; 
                shortest_path(NodeMap, NodeList, WayList, n1, n2);
                cout<<line<<endl;
                break;
            }
            default: break;
        }
    }
    return 0;
}
/*
3
2235336824
2235354622

3
1068901807
1361111383

3
577489927
600645120

3
2274115409
2274115384

3
1961375527
1961374932

3
3258023129
9236071057
*/