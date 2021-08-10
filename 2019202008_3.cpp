#include <stdio.h>
#include <string.h>
#include "mpi.h"
#include <fstream>
#include <bits/stdc++.h>
using namespace std;
typedef long long int ll;

bool isSatisfy(int graph[][3], int i, int j)
{
    if(graph[i][0] == graph[j][0])
        return true;
    else if(graph[i][1] == graph[j][0])
        return true;
    else if(graph[i][0] == graph[j][1])
        return true;
    else if(graph[i][1] == graph[j][1])
        return true;
    else
        return false;
}

void edgecolor(int start, int end, int edgenum, int graph[][3]) 
{ 
	int color; 
	for (int i = 0; i < edgenum; i++)
    { 
		color = 1;
        if(graph[i][2] == 0 && graph[i][0] >= start && graph[i][1] <= end)
        {
        flag:  
            graph[i][2] = color; 
            for (int j = 0; j < edgenum; j++) 
            { 
                if(j != i)  
                {
                    if(isSatisfy(graph,i,j) && graph[j][0] >= start && graph[j][1] <= end) 
                    { 
                        if (graph[i][2] == graph[j][2])
                        { 
                            color++;
                            goto flag; 
                        } 
                    }
                } 
            } 
        }
	}
} 

int main( int argc, char **argv ) {
    int rank, numprocs;

    /* start up MPI */
    MPI_Init( &argc, &argv );

    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    MPI_Comm_size( MPI_COMM_WORLD, &numprocs );
    
    /*synchronize all processes*/
    MPI_Barrier( MPI_COMM_WORLD );
    double tbeg = MPI_Wtime();

    /* write your code here */
    if(rank == 0)
    {
        int vertexnum, edgenum;
        ifstream infile(argv[1]);
        ofstream outfile(argv[2]);
        string fline;
        getline(infile, fline);
        istringstream iss(fline);
        int temp_array[2];
        int i = 0;
        for(string s;iss>>s;)
        {
            temp_array[i] = stoll(s,nullptr,10);
            i++;
        }
        vertexnum = temp_array[0];
        edgenum = temp_array[1];
        int graph[edgenum][3];
        int colors[edgenum];
        int count = 0;
        while(getline(infile,fline))
        {
            int i = 0;
            istringstream iss(fline);
            for(string s;iss>>s;)
            {
                temp_array[i] = stoll(s,nullptr,10);
                i++;
            }
            int x = temp_array[0];
            int y = temp_array[1];
            graph[count][0] = x;
            graph[count][1] = y;
            graph[count][2] = 0;
            count++;
        }
        // cout<<"vertex "<<vertexnum<<" edge "<<edgenum<<endl;
        // for(int i=1; i<=vertexnum; i++)
        // {
        //     for(int j=1; j<=vertexnum; j++)
        //     {
        //         cout<<graph[i][j]<<" ";
        //     }
        //     cout<<endl;
        // }
        if(numprocs == 1)
        {
                edgecolor(1, vertexnum, edgenum, graph);
        }
        else
        {
            double a = double(vertexnum)/double(numprocs-1);
            int window = floor(a);
            int start = 1;
            int end;
            // cout<<" window "<<window<<endl;
            if(window == 0)
            {
                for(int i=1; i<numprocs; i++)
                {
                    if(i == 1)
                    {
                        // cout<<"here1"<<endl;
                        MPI_Send(&start,1,MPI_INT,i,0,MPI_COMM_WORLD);
                        MPI_Send(&vertexnum,1,MPI_INT,i,0,MPI_COMM_WORLD);
                        MPI_Send(&vertexnum,1,MPI_INT,i,0,MPI_COMM_WORLD);
                        MPI_Send(&edgenum,1,MPI_INT,i,0,MPI_COMM_WORLD);
                        MPI_Send(graph,edgenum*3,MPI_INT,i,0,MPI_COMM_WORLD);

                        int temp_colors[edgenum][3];
                        MPI_Recv(temp_colors,edgenum*3,MPI_INT,i,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
                        for(int i=0;i<edgenum; i++)
                        {
                            graph[i][2] = temp_colors[i][2];
                        }
                    }
                    else
                    {
                        // cout<<"here2"<<endl;
                        int temp = 0;
                        int temp_colors[edgenum][3];
                        MPI_Send(&start,1,MPI_INT,i,0,MPI_COMM_WORLD);
                        MPI_Send(&temp,1,MPI_INT,i,0,MPI_COMM_WORLD);
                        MPI_Send(&vertexnum,1,MPI_INT,i,0,MPI_COMM_WORLD);
                        MPI_Send(&edgenum,1,MPI_INT,i,0,MPI_COMM_WORLD);
                        MPI_Send(temp_colors,edgenum*3,MPI_INT,i,0,MPI_COMM_WORLD);

                        MPI_Recv(temp_colors,edgenum*3,MPI_INT,i,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
                    }
                }
            }
            else
            {
                for(int i=1;i<numprocs;i++)
                {
                    end = start+window;
                    // cout<<"Start "<<start<<" end "<<end<<endl;
                    if(end <= vertexnum)
                    {
                        MPI_Send(&start,1,MPI_INT,i,0,MPI_COMM_WORLD);
                        MPI_Send(&end,1,MPI_INT,i,0,MPI_COMM_WORLD);
                        MPI_Send(&vertexnum,1,MPI_INT,i,0,MPI_COMM_WORLD);
                        MPI_Send(&edgenum,1,MPI_INT,i,0,MPI_COMM_WORLD);
                        MPI_Send(graph,edgenum*3,MPI_INT,i,0,MPI_COMM_WORLD);
                        int temp_colors[edgenum][3];
                        MPI_Recv(temp_colors,edgenum*3,MPI_INT,i,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
                        // cout<<"after receive"<<endl;
                        for(int i=0;i<edgenum; i++)
                        {
                            graph[i][2] = temp_colors[i][2];
                        }
                    }
                    else
                    {
                        end = vertexnum;
                        // cout<<"end here "<<end<<endl;

                        MPI_Send(&start,1,MPI_INT,i,0,MPI_COMM_WORLD);
                        MPI_Send(&end,1,MPI_INT,i,0,MPI_COMM_WORLD);
                        MPI_Send(&vertexnum,1,MPI_INT,i,0,MPI_COMM_WORLD);
                        MPI_Send(&edgenum,1,MPI_INT,i,0,MPI_COMM_WORLD);
                        MPI_Send(graph,edgenum*3,MPI_INT,i,0,MPI_COMM_WORLD);
                        int temp_colors[edgenum][3];
                        MPI_Recv(temp_colors,edgenum*3,MPI_INT,i,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
                        // cout<<"after receive"<<endl;
                        for(int i=0;i<edgenum; i++)
                        {
                            graph[i][2] = temp_colors[i][2];
                        }
                    }
                    start = end+1;
                }
                // cout<<"before final call"<<endl;
                edgecolor(1, vertexnum, edgenum, graph);
                // cout<<"after final call"<<endl;
            }
        }
        int maxcolor = 0;
        for(int i=0; i<edgenum; i++)
        {
            maxcolor = max(maxcolor,graph[i][2]);
        }
        outfile<<maxcolor<<"\n";
        for(int i=0;i<edgenum; i++)
        {
            // cout<<"why"<<endl;
            outfile<<graph[i][2]<<" ";
        }
    }
    else
    {
        int vertexnum, edgenum, start, end;
        MPI_Recv(&start,1,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        MPI_Recv(&end,1,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        MPI_Recv(&vertexnum,1,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        MPI_Recv(&edgenum,1,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        // cout<<"start "<<start<<" end "<<end<<" vertexnum "<<vertexnum<<" edgenum "<<edgenum<<endl;
        int graph[edgenum][3];
        MPI_Recv(graph,edgenum*3,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        if(end == 0)
        {
            // cout<<"when window zero"<<endl;
            MPI_Send(graph,edgenum*3,MPI_INT,0,0,MPI_COMM_WORLD);
        }
        else
        {
            edgecolor(start, end, edgenum, graph);
            // cout<<"after calculation"<<endl;
            MPI_Send(graph,edgenum*3,MPI_INT,0,0,MPI_COMM_WORLD);
            // cout<<"after send"<<endl;
        }
        
    }
    // cout<<"before barrier"<<endl;
    MPI_Barrier( MPI_COMM_WORLD );
    // cout<<"after barrier"<<endl;
    double elapsedTime = MPI_Wtime() - tbeg;
    double maxTime;
    MPI_Reduce( &elapsedTime, &maxTime, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD );
    if ( rank == 0 ) {
        printf( "Total time (s): %f\n", maxTime );
    }

    /* shut down MPI */
    MPI_Finalize();
    return 0;
}