#include <cstdlib>
#include <iostream>
#include <math.h>

#include <Mesh.h>

using namespace glm;
using namespace std;


Mesh::Mesh(const char* filename) 
{
	int j = 0;
    unsigned int tmp;
	FILE *file;
    int   error;

    if((file=fopen(filename,"r"))==NULL)
	{
		std::cerr << "Unable to read : " << filename << std::endl;
	}

	// create mesh
    vertices = vector<vec3>();
    faces    = vector<vector< unsigned int > >();

    unsigned int nb_vertices, nb_faces;

	error = fscanf(file,"OFF\n%d %d %d\n",&(nb_vertices),&(nb_faces),&tmp);
	if(error==EOF) 
	{
		std::cerr << "Unable to read : " << filename << std::endl;
	}

    vertices.resize(nb_vertices);
    faces.resize(nb_faces);

	// reading vertices
	for(int i=0;i<nb_vertices;++i) 
	{
		error = fscanf(file,"%f %f %f\n",&(vertices[i][0]),&(vertices[i][1]),&(vertices[i][2]));
		if(error==EOF) 
		{
			std::cerr << "Unable to read vertices of : " << filename << std::endl;
			exit(EXIT_FAILURE);
		}
	}

	// reading faces
	j = 0;
    for(int i = 0; i < nb_faces; ++i)
	{
        error = fscanf(file,"%d ",&tmp);
		
		if(error==EOF) 
		{
			std::cerr << "Unable to read faces of : " << filename << std::endl;
			exit(EXIT_FAILURE);
        }

        unsigned int N = tmp;

        for(unsigned int j = 0; j < N; j++)
        {
            error = fscanf(file,"%d ",&tmp);

            if(error==EOF)
            {
                std::cerr << "Unable to read faces of : " << filename << std::endl;
                exit(EXIT_FAILURE);
            }

            faces[i].push_back(tmp);
        }
	}

	fclose(file);
}


vec3 Mesh::get_vertex(const unsigned int i) const
{
    return vertices[i];
}


vector< unsigned int > Mesh::get_face(const unsigned int i) const
{
    return faces[i];
}


void Mesh::write(const char* filename) const 
{
	FILE *file;

    // file opening
	if((file=fopen(filename,"w"))==NULL) 
	{
		std::cerr << "Unable to open : " << filename << std::endl;
	}
	
	
	// header
	fprintf(file, "OFF\n");
    fprintf(file,"%lu %lu\n", vertices.size(), faces.size());
	
	// vertices positions
    for(unsigned int i = 0; i < vertices.size(); i++)
    {
        vec3 p = get_vertex(i);
        fprintf(file,"%f %f %f\n", p[0], p[1], p[2]);
    }
    
    // faces indices
    for(unsigned int i = 0; i < faces.size(); i++)
    {
        vector< unsigned int > f = get_face(i);
        fprintf(file,"3 %i %i %i\n", f[0], f[1], f[2]);
    }
}


void Mesh::write_obj(const char* filename) const 
{
	FILE *file;

    // file opening
	if((file=fopen(filename,"w"))==NULL) 
	{
		std::cerr << "Unable to open : " << filename << std::endl;
	}
	
	
	// vertices positions
    for(unsigned int i = 0; i < vertices.size(); i++)
    {
        vec3 p = get_vertex(i);
        fprintf(file,"v %f %f %f\n", p[0], p[1], p[2]);
    }
    
    // faces indices
    for(unsigned int i = 0; i < faces.size(); i++)
    {
        vector< unsigned int > f = get_face(i);
        fprintf(file,"f");

        for(unsigned int j = 0; j < f.size(); j++)
        {
            fprintf(file," %i", f[j]+1);
        }

        fprintf(file,"\n");
    }
}



unsigned int edges_common_face(unsigned int i_e0, unsigned int i_e1, vector< vector<unsigned int> > edge_faces)
{
    // for all faces adjacent to edge i_e0
    for(unsigned int i = 0; i < edge_faces[i_e0].size(); i++)
    {
        // and for all faces adjacent to edge i_e1
        for(unsigned int j = 0; j < edge_faces[i_e1].size(); j++)
        {
            // return identical face index
            if(edge_faces[i_e0][i] == edge_faces[i_e1][j])
                return edge_faces[i_e0][i];
        }
    }
    
    // no common face has been found -> error display
    cerr << "common face not found between edges " << i_e0 << " and " << i_e1 << "." << endl;
    
    return 0;
}

unsigned int vertices_common_face(unsigned int i0, unsigned int i1, unsigned int i2, vector< vector<unsigned int> > vert_faces)
{
    // for all faces adjacent to vertex i0
    for(unsigned int i = 0; i < vert_faces[i0].size(); i++)
    {
        // and for all faces adjacent to vertex i1
        for(unsigned int j = 0; j < vert_faces[i1].size(); j++)
        {
            // if thay have a common face
            if(vert_faces[i0][i] == vert_faces[i1][j])
            {
                // for all faces adjacent to vertex i2
                for(unsigned int k = 0; k < vert_faces[i2].size(); k++)
                {
                    // returns the common face
                    if(vert_faces[i0][i] == vert_faces[i2][k])
                        return vert_faces[i0][i];
                }
            }
        }
    }
    
    // no common face has been found -> error display
    cerr << "common face not found between vertices " << i0 << ", " << i1 << ", and " << i2 << "." << endl;
    
    return 0;
}

vec3 Mesh::calculerBarycentreFace(vector< unsigned int > f) const{
   vec3 barycentre = vec3(0.0,0.0,0.0);
   vec3 tmp;
    for(int i=0;i<f.size();i++){
       tmp = vertices.at(f.at(i));
       barycentre = vec3(barycentre.x+(tmp.x/f.size()),barycentre.y+(tmp.y/f.size()),barycentre.z+(tmp.z/f.size()));
    }
    return barycentre;
}

vec3 Mesh::calculerBarycentreTetra(vector< vec3 > f) const{
   vec3 barycentre = vec3((float)0,(float)0,(float)0);
   vec3 tmp;
    for(int i=0;i<f.size();i++){
       tmp = f.at(i);
       barycentre = vec3(barycentre.x+(tmp.x/f.size()),barycentre.y+(tmp.y/f.size()),barycentre.z+(tmp.z/f.size()));
    }
    return barycentre;
}



vec3 Mesh::deplacement(unsigned int sommet ,vec3 S,vector<vec3> listefsi,vector<vec3>listeSai) const {

       unsigned int n= vertices.size();
      vector< Edge > edges= get_edges();
       //
      vector< vector< unsigned int > > edgesAdjacentes=get_vertex_edges (edges) ;

      vector< vector< unsigned int > >  facesAdjacentes = get_vertex_faces();

     vector< Edge >  aretes=get_edges();
     vector< vector<unsigned int> > voisins=get_neighborhoods();

        vector< vector< unsigned int > > listeEdge=get_vertex_edges(voisins,aretes);
        vector<unsigned int> face=facesAdjacentes.at(sommet);
        vector<vec3> listSFI;
        for(int j=0;j<face.size();j++){
            int ii=(int)face.at(j);
            listSFI.push_back(listefsi.at(ii));
        }
            vec3 resultF=calculerBarycentreTetra(listSFI);


        vector< unsigned int > listEd=listeEdge.at(sommet);
        vec3 resultA=calculerBarycentreTetra(listeSai);
        //calcul de A*2

        vec3 resultA_2=vec3(resultA.x*2,resultA.y*2,resultA.z*2);
        //calcul de (n-3)*S
        vec3 S_n_1=vec3(S.x*(n-1),S.y*(n-1),S.z*(n-1));
        //calcul du résultat snouv
        float v1=(resultF.x+2*resultA_2.x+S_n_1.x)/2;
        float v2=(resultF.y+2*resultA_2.y+S_n_1.y)/2;
        float v3=(resultF.z+2*resultA_2.z+S_n_1.z)/2;
        vec3 newS=vec3(v1,v2,v3);

        return newS;
}



Mesh Mesh::subdivide() const
{
    Mesh output;
    //Calcul barycentre Sf de chaque face
    vector<vec3 > sf;

    cout << "debut SF"<<endl;
    //SF OK !
     cout << faces.size()<<endl;
     for(int i=0;i<faces.size();i++){
            sf.push_back(calculerBarycentreFace(get_face(i)));
    }

    //Calcul de Sa pour chaque arête a
     //problemes dans get_edges : seg fault
     vector<Edge > aretes = get_edges();
     vector <vector < unsigned int > > faces_voisines = get_edge_faces(aretes);
     vector<vec3 > tetra;
     vector<vec3 > sa;


     cout << "debut SA"<<endl;

     for(int i=0;i<faces_voisines.size();i++){
         /*Extremites de l'arête*/
         cout<<"   "<<endl;
         cout<<"i "<<endl;
         cout << (aretes.at(i).m_i0) <<endl;
         cout << (aretes.at(i).m_i1) <<endl;
         tetra.push_back(get_vertex(aretes.at(i).m_i0));
         tetra.push_back(get_vertex(aretes.at(i).m_i1));
         for(int j=0;j<faces_voisines.at(i).size();j++){
             /*faces adjacentes*/
             tetra.push_back(sf.at(faces_voisines.at(i).at(j)));
         }
         sa.push_back(calculerBarycentreTetra(tetra));
         tetra.clear();
     }

     cout << "debut deplacement"<<endl;

    //Deplacement de S à tester
     vector <vec3 > sommets;
     for(int i=0;i<vertices.size();i++){
         sommets.push_back(deplacement(i,vertices.at(i),sf,sa));

     }
     cout << "fin deplacement"<<endl;

    //Formation des faces

    //On forme les faces
    vector< vector< unsigned int> > faces_f = get_vertex_faces();
    vector< vector< unsigned int> > aretes_a = get_vertex_edges(aretes);
    vector< unsigned int > sa_i;
    vector< unsigned int > faces_s;
    vector< unsigned int > aretes_s;
    vector< unsigned int > f;
    //On ajoute les points :
    for(int i=0;i<sommets.size();i++){
        output.vertices.push_back(sommets.at(i));
    }
    for(int i=0;i<sa.size();i++){
        output.vertices.push_back(sa.at(i));
    }
    for(int i=0;i<sf.size();i++){
        output.vertices.push_back(sf.at(i));
    }
    for(int i=0;i<sommets.size();i++){//pour chque sommet
        faces_s  = faces_f.at(i);//on recupere les faces voisines du sommet i
        aretes_s = aretes_a.at(i); //on recupere les aretes voisines du sommet i

        for(int j=0;j<faces_s.size();j++){//pour chaque face on cree la nouvelle face avec
            for(int k=0;k<aretes_s.size();k++){//pr chque arete voisine
                //si elle appartient a cette face :
               for(int t=0;t<4;t++){
                    if((faces.at(faces_s.at(j)).at(t)!= i)
                            &&((faces.at(faces_s.at(j)).at(t)==aretes.at(aretes_s.at(k)).m_i0)||(faces.at(faces_s.at(j)).at(t)== aretes.at(aretes_s.at(k)).m_i1))){
                        sa_i.push_back(aretes_s.at(k));
                    }
                }
            }

            f.push_back(i);
            f.push_back(sommets.size()+sa_i.at(0));
            f.push_back(sommets.size()+sa.size()+faces_s.at(j));
            f.push_back(sommets.size()+sa_i.at(1));
            output.faces.push_back(f);
            f.clear();
            sa_i.clear();
        }

    }
    cout <<"points "<<endl;
    cout<<" "<<endl;
    for(int i=0;i<output.vertices.size();i++){
        cout<<"i "<<output.vertices.at(i).x<<" "<<output.vertices.at(i).y<<" "<<output.vertices.at(i).z<<endl;
    }
    cout<<" "<<endl;
    cout<<"faces "<<endl;
    cout<<" "<<endl;
    for(int i=0;i<output.faces.size();i++){
        cout<<" i "<<output.faces.at(i).at(0)<<" "<<output.faces.at(i).at(1)<<" "<<output.faces.at(i).at(2)<<" "<<output.faces.at(i).at(3)<<endl;

    }


    //=======================================================
    //
    // TODO : implémenter le schema de subdivision de Catmull-Clark
    //
    //=======================================================



   //output = *this;     // place holder : current mesh copy

    return output;
}



vector< vector<unsigned int> > Mesh::get_neighborhoods() const
{
    vector< vector<unsigned int> > output;
    
    for(unsigned int i = 0; i < vertices.size(); i++)
    {
        output.push_back(vector< unsigned int >());
    }
    
    
    // unordered neighborhood computation
    for(unsigned int i = 0; i < faces.size(); i++)
    {
        vector< unsigned int > f = get_face(i);
        for(unsigned int j=0; j < f.size(); j++)
        {
            unsigned int next = f[(j+1)%f.size()];
            if(std::find(output[f[j]].begin(), output[f[j]].end(), next) == output[f[j]].end())
                output[f[j]].push_back(next);

            next = f[(j+f.size()-1)%f.size()];
            if(std::find(output[f[j]].begin(), output[f[j]].end(), next) == output[f[j]].end())
                output[f[j]].push_back(next);
        }
    }
    

    // neighborhood ordering
    vector< vector< unsigned int > > output2;
    for(unsigned int i = 0; i < vertices.size(); i++)
    {
        output2.push_back(vector< unsigned int >());
        
        output2[i].push_back(output[i][0]);
        for(unsigned int j = 1; j < output[i].size(); j++)
        {
            unsigned int i_prev = i;
            unsigned int i_next = output2[i][j-1];

            while(i_next != i)
            {
                unsigned int temp = i_next;
                i_next = get_next_vert(i_prev, i_next);
                i_prev = temp;
            }


            output2[i].push_back(i_prev);
        }
    }
    
    return output2;
}


std::vector< Edge > Mesh::get_edges() const
{
    vector< vector< unsigned int > > neib = get_neighborhoods();
    vector< Edge> output;
    
    // for all vertices
    for(unsigned int i = 0; i < neib.size(); i++)
    {
        // for all neighbors
        for(unsigned int j=0; j < neib[i].size(); j++)
        {
            Edge e(i, neib[i][j]);
            
            // add the edge if it is not already created
            if(std::find(output.begin(), output.end(), e) == output.end())
                output.push_back(e);
        }
    }
    
    return output;
}


vector< vector< unsigned int > > Mesh::get_vertex_faces() const
{
    vector< vector< unsigned int > > output;
    
    // for all vertices
    for(unsigned int i = 0; i < vertices.size(); i++)
    {
        // create an empty incident face set
        output.push_back(vector< unsigned int >());
    }
    
    // for all faces
    for(unsigned int i = 0; i < faces.size(); i++)
    {
        vector< unsigned int > f = get_face(i);
        
        // for all composing vertices
        for(unsigned int j=0; j < f.size(); j++)
        {
            // add self as incident
            output[f[j]].push_back(i);
        }
    }
    
    return output;
}


vector< vector< unsigned int > > Mesh::get_vertex_edges(vector< Edge > edges) const
{
    vector< vector< unsigned int > > output;
    
    // for all vertices
    for(unsigned int i = 0; i < vertices.size(); i++)
    {
        // create an empty incident edge set
        output.push_back(vector< unsigned int >());
    }
    
    // for all edges
    for(unsigned int i = 0; i < edges.size(); i++)
    {
        // add self as incident to both extremities
        output[edges[i].m_i0].push_back(i);
        output[edges[i].m_i1].push_back(i);
    }
    
    return output;
}


vector< vector< unsigned int > > Mesh::get_vertex_edges(vector< vector< unsigned int > > vert_neib, vector< Edge > edges) const
{
    vector< vector< unsigned int > > output;
    
    // for all vertices
    for(unsigned int i = 0; i < vertices.size(); i++)
    {
        // add an empty incident edges set
        output.push_back(vector< unsigned int >());
        
        // for all incident edges
        for(unsigned int j = 0; j < vert_neib[i].size(); j++)
        {
            // add corresponding edge index
            unsigned int k = std::find(edges.begin(), edges.end(), Edge(i, vert_neib[i][j])) - edges.begin();
            output[i].push_back(k);
        }
    }
    
    return output;
}


vector< vector< unsigned int > > Mesh::get_edge_faces(vector< Edge > edges) const
{
    vector< vector< unsigned int > > vf = get_vertex_faces();
    
    vector< vector< unsigned int > > output;
    
    // for all edges
    for(unsigned int i = 0; i < edges.size(); i++)
    {
        // add an empty incident face set
        output.push_back(vector< unsigned int >());
        
        // retrieve extremities indices
        unsigned int p0 = edges[i].m_i0;
        unsigned int p1 = edges[i].m_i1;
        
        // for all faces adjacent to one extremity
        for(unsigned int j0 = 0; j0 < vf[p0].size(); j0++)
        {
            // and for all faces adjacent to the other extremity
            for(unsigned int j1 = 0; j1 < vf[p1].size(); j1++)
            {
                // add identical faces
                if(vf[p0][j0] == vf[p1][j1])
                    output[i].push_back(vf[p0][j0]);
            }
        }
    }
    
    return output;
}


unsigned int Mesh::get_oriented_face(unsigned int i_e0, unsigned int i_e1) const
{
    // for all faces
    for(unsigned int i = 0; i < faces.size(); i++)
    {
        vector< unsigned int > f = get_face(i);
        
        // for all composing vertices
        for(unsigned int j=0; j < f.size(); j++)
        {
            // return the right face
            if(f[j] == i_e0 && f[(j+1)%f.size()] == i_e1)
                return i;
        }
    }
    
    // no common face has been found -> error display
    cerr << "oriented face not found for edge " << i_e0 << " -> " << i_e1 << "." << endl;
    
    return 0;
}

unsigned int Mesh::get_next_vert(unsigned int i_e0, unsigned int i_e1) const
{
    // for all faces
    for(unsigned int i = 0; i < faces.size(); i++)
    {
        vector< unsigned int > f = get_face(i);

        // for all composing vertices
        for(unsigned int j = 0; j < f.size(); j++)
        {
            // return the following vertex index in the right face
            if(f[j] == i_e0 && f[(j+1)%f.size()] == i_e1)
                return f[(j+2)%f.size()];
        }
    }
    
    // no common face has been found -> error display
    cerr << "Next vertex not found for edge " << i_e0 << " -> " << i_e1 << "." << endl;
    
    return 0;
}


