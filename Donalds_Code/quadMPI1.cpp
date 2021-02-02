//
// prlong longs what processor it is out of the total number of processors from all ranks
// and finds the closest multiple of three to its rank
//
//mpic++ -o quadMPI1 quadMPI1.cpp && mpirun -np 10 ./quadMPI1
#include <iostream>
#include <mpi.h>
#include <math.h>
#include <sys/time.h>
//#include <ctime>
using namespace std;
double f(double x,double y){

  return x*sin(x*x)+y*sin(y*y);
}
double quad(double ps[2], double s[2]){
  double d[3]={0.1127016653792583,0.5,0.8872983346207417};
  double  x[3];
  double  y[3];
  double corner=0.1388888888888889;
  double center=0.4444444444444444;
  for (long long i=0; i<3 ; i++){
    x[i]=ps[0]+s[0]*d[i];
    y[i]=ps[1]+s[1]*d[i];
  }
  return corner*(f(x[0],y[0])+f(x[0],y[2])+f(x[2],y[0])+f(x[2],y[2]))+center*f(x[1],y[1]);

}
double * piece(double  absc[2], long long step[2], long long id ){
  long long loc[2]={id%step[0], id/step[0]};
  //cout << "loc "<<loc[0]<<","<<loc[1]<<endl;
  static double out[2];
  out[0]=absc[0]*(double (loc[0]));
  out[1]=absc[1]*(double (loc[1]));
  //cout << "out = ["<<loc[0]<<","<<out[0]<<","<<out[1]<<","<<absc[0]<<"]"<<endl;
  return out;

}
void findsum(double ab[2], long long step[2], long long rank, long long size){
  MPI_Barrier(MPI_COMM_WORLD);
  time_t timer=time(&timer);
  double absc[2]={ab[0]/double( step[0]), ab[1]/double (step[1])};
  long long pieces=step[0]*step[1];
  long long lim[2]={pieces*rank/size,pieces*(rank+1)/size};
  double local_sum=0.;
  for (long long i=lim[0];i<lim[1];i++){

    local_sum+=quad(piece(absc,step,i),absc);
  }
  double global_sum=0.;
  //local_sum=1.;
  MPI_Reduce(&local_sum, &global_sum, 1, MPI_DOUBLE, MPI_SUM, 0,MPI_COMM_WORLD);
  if (rank==0){
    cout << "raw_sum "<<global_sum<<" "<<endl;
    cout << "Used "<<size<<" threads"<<endl;
    cout << "ab = ["<<ab[0]<<","<<ab[1]<<"]"<<endl;
    cout << "step = ["<<step[0]<<","<<step[1]<<"]"<<endl;
    global_sum*=(ab[0]*ab[1])/double(pieces);
    cout << "scale "<<(ab[0]*ab[1])/double(pieces)<<" "<<endl;
    cout << "pieces "<<pieces<<" "<<endl;
    cout << "quadrature integral = "<<global_sum<<endl;
    double actual=0.5*(-ab[1]*cos(ab[0]*ab[0])-ab[0]*cos(ab[1]*ab[1])+ab[0]+ab[1]);
    cout << "actual integral = "<<actual<<endl;
    cout << " integral error= "<<global_sum-actual<<endl;
    double seconds=difftime(time(NULL),timer);
    cout << "taking "<< seconds <<" seconds"<<endl;
    cout << "taking "<< seconds/double(pieces) <<" seconds per quad"<<endl;
  }
  //cout << "red "<<rank<<","<<local_sum<<","<<global_sum<<endl;
}
int main(){
  int rank, size, ierr;
  MPI_Comm comm;

  comm  = MPI_COMM_WORLD;

  MPI_Init(NULL,NULL);
  MPI_Comm_rank(comm, &rank);
  MPI_Comm_size(comm, &size);
  double ab[2]={100.,100.};
  long long step[2];//={10000,10000};
  //long long pieces=step[0]*step[1];
  //long long lim[2]={pieces*rank/size,pieces*(rank+1)/size};
  //cout << "lim "<<lim[0]<<","<<lim[1]<<endl;
  //for (long long i=lim[0];i<lim[1];i++){
  //  piece(ab,step,i);
  //}
  long long resolutions[]={10000,33333,100000,333333,1000000};
  //findsum(ab,step,rank,size);
  for (int i=0;i<5;i++){
    step[0]=step[1]=resolutions[i];
    findsum(ab,step,rank,size);
  }
  //cout << "Quad "<<quad(0.,0.,1.)<<endl;
  // Find the closest multiple of three to your rank below ...

  // Each process prlong longs out its rank
  //cout << "I am "<<rank<<" out of "<<size<<" and closest multiple of 3 to me is ..."<<endl;

  MPI_Finalize();

}
