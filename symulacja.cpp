#include <iostream>
#include <fstream>
#include <chrono> //timer
#include <math.h>
using namespace std;
using namespace std::chrono;

const float g = 9.81;
const float pi = 3.14159;

struct Vec3
{
    double x,y,z;
};
struct Vec2
{
    double x,y;
};

// wzor na sile oporu 0,5*cd*p*a*v^2

struct CanSat
{
    Vec3 pos;
    Vec3 vel;
    double Cd;
    double A;
    double mass;
    Vec3 acc;
    Vec2 rot;  //rotacja wyliczana z f. tryg
    Vec3 F;    //sila dla kazdej z osi
};

double isa_air_density(double H){
    double p;
    p = 1.2255*pow((1-(H/44300)),4.256);
    return p;
}

int main(){
    ofstream plik;
    plik.open("trajektoria.out", ios::out);
    auto start = std::chrono::steady_clock::now();
    double x,y,z;
    double vx,vy,vz;
    cout<<"Podaj pozycje startowa (X,Y,Z): ";
    cin>>x,y,z;
    cout<<endl<<"Podaj predkosc startowa (X,Y,Z): ";
    cin>>vx,vy,vz;
    cout<<endl<<"Podaj krok obliczen dt = ";
    double dt = 0.01;
    cin>>dt;
    cout<<endl;
    plik<<"X,"<<"Y,"<<"Z,"<<"VX,"<<"VY,"<<"VZ,"<<"RX,"<<"RY,"<<"TIME,"<<endl;
    CanSat TerraCan = {x,y,z, vx,vy,vz, 0.8,0.0045, 0.32}; // pos, vel, Cd, A,  masa
    double V_total, Fd, p; //predkosc wypadkowa, wypadkowy opor powietrza, gestosc powietrza dla aktualnej wysokosci
    const double Fg = -(TerraCan.mass * g); // sila ciezkosci jest stala
    for (double t = 0; t < 1000; t+=dt)
    {
        V_total = sqrt(pow(TerraCan.vel.x,2)+pow(TerraCan.vel.y,2)+pow(TerraCan.vel.z,2));  // wartosc wektora predkosci - predkosc wypadkowa
        p = isa_air_density(TerraCan.pos.z);

        if(t>10){                                                                           // warunek otwarcia spadochronu
            Fd = 0.5*TerraCan.Cd*p*TerraCan.A*pow(V_total,2) + 0.5*0.8*p*0.1*pow(V_total,2);//wypadkowy opor powietrza
        }else{
            Fd = 0.5*TerraCan.Cd*p*TerraCan.A*pow(V_total,2);
        }

        if(V_total > 0){                                                                    // w pierwszej sekundzie symulacji predkosc = 0 - zabezpieczenie przed /0
            TerraCan.F.x = 0 - (TerraCan.vel.x/V_total)*Fd;                                 //skalowanie oporu powietrza dla kazdej z osi proporcja do predkosci
            TerraCan.F.y = 0 - (TerraCan.vel.y/V_total)*Fd;
            TerraCan.F.z = Fg - (TerraCan.vel.z/V_total)*Fd;
        } else {
            TerraCan.F.x = 0;
            TerraCan.F.y = 0;
            TerraCan.F.z = Fg;
        }

        //obliczanie kolejno przyspieszenia w danej sekundzie, predkosci calkowitej dla kazdej z osi, oraz przemieszczenia

        TerraCan.acc.x = TerraCan.F.x/TerraCan.mass;
        TerraCan.acc.y = TerraCan.F.y/TerraCan.mass;
        TerraCan.acc.z = TerraCan.F.z/TerraCan.mass;
        
        TerraCan.vel.x += TerraCan.acc.x * dt;
        TerraCan.vel.y += TerraCan.acc.y * dt;
        TerraCan.vel.z += TerraCan.acc.z * dt;

        //obliczanie pochylenia puszki z tg

        if(TerraCan.vel.x == 0){
            TerraCan.rot.y = 90;
        } else{
            TerraCan.rot.y = abs(atan(TerraCan.vel.z/TerraCan.vel.x)) * (180/pi); //tg kata w osi y
        }

        if(TerraCan.vel.y == 0){
            TerraCan.rot.x = 90;
        } else{
            TerraCan.rot.x = abs(atan(TerraCan.vel.z/TerraCan.vel.y)) * (180/pi); //tg kata w osi x
        }
        

        TerraCan.pos.x += TerraCan.vel.x * dt;
        TerraCan.pos.y += TerraCan.vel.y * dt;
        TerraCan.pos.z += TerraCan.vel.z * dt;

        cout<<"Pozycja X: "<<TerraCan.pos.x<<"m Y: "<<TerraCan.pos.y<<"m Z: "<<TerraCan.pos.z<<" m"<<" Rotacja X: "<<TerraCan.rot.x<<"st Y: "<<TerraCan.rot.y<<"st Predkosc pionowa:"<<TerraCan.vel.z<<" m/s W sekundzie: "<<t<<"s"<<endl;
        plik<<TerraCan.pos.x<<","<<TerraCan.pos.y<<","<<TerraCan.pos.z<<","<<TerraCan.vel.x<<","<<TerraCan.vel.y<<","<<TerraCan.vel.z<<","<<TerraCan.rot.x<<","<<TerraCan.rot.y<<","<<t<<endl;
        if(TerraCan.pos.z < 0 ) break;
    }
    plik.close();
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << "Czas wykonania: " << elapsed.count() << " ms\n";

    
    return 0;
}