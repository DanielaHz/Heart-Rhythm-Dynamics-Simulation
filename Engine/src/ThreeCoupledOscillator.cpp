#include "ThreeCoupledOscillator.h"

void HeartOscillatorSystem::update( double dt)
{
    // SA Node
    double x1 = sa.x;
    double x2 = sa.dx;
    double x3 = av.x;
    double x5 = hp.x;
    
    double dx1 = x2;
    double dx2 = -sa.a* x2 * (x1 - sa.w1) * (x1 - sa.w2) - x1 * (x1 + sa.d) * (x1 + sa.e) + sa.q* glm::sin(sa.omega * t) + sa.kSA_to_AV * (x1 - x3) + sa.kSA_to_HP * (x1 - x5);

    // AV Node
    double x4 = av.dx;
    double x6 = hpc.dx;

    double dx3 = x4;
    double dx4 = -av.a* x4 * (x3 - av.w1) * (x2 - av.w2) - x3 * (x3 + av.d) * (x3 + av.e) + av.q* glm::sin(av.omega * t) + av.kAV_to_SA * (x3 - x1) + av.kAV_to_HP * (x3 - x5);

    // HisPurkinjeComplex
    double dx5 = x6;
    double dx6 = -hpc.a* x6 * (x5 - hpc.w1) * (x5 - hpc.w2) - x5 * (x5 + hpc.d) * (x5 + hpc.e) + hpc.q* glm::sin(hpc.omega * t) + hpc.kHP_to_SA * (x5 - x1) + hpc.kHP_to_AV * (x5 - x3);

}   

double HeartOscillatorSystem::getECG()
{
    return a0 + a1*sa.x +  a3 * av.x + a5 * hpc.x;
}