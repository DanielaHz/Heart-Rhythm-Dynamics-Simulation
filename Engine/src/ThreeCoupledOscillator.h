# pragma once

// This model was taking from the paper called: An analysis of heart rhythm dynamics using a three-coupled oscillator model
// Authors: Sandra R.F.S.M Gois & Marcelo A. Savi

struct SANode
{
    double x; // principal state (x1)
    double dx; // derivate (x2)

    // parameters for the SA node
    double a,d,e,w1,w2;
    double q, omega;
    double kSA_to_AV, kSA_to_HP;
}

struct AVNode
{
    double x; // principal state (x3)
    double dx; // derivate (x4)

    // parameters for the AV node
    double a,d,e,w1,w2;
    double q, omega;
    double kAV_to_SA, kAV_to_HP;
}

struct HisPurkinjeComplex
{
    double x; // principal state (x5)
    double dx; // derivate (x6)

    // parameters for the AV node
    double a,d,e,w1,w2;
    double q, omega;
    double kHP_to_SA, kHP_to_AV;
}

struct HeartOscillatorSystem
{
    SANode sa;
    AVNode av;
    HisPurkinjeComplex hpc;

    double t; // time

    void update(double dt);
    double getECG();
}
