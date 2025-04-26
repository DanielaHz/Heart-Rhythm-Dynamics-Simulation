# pragma once

#include <glm/glm.hpp>

// This class was took from the NGL MassSpring DEMO available and I adapted to the context of this project.
// link to the demo: https://github.com/NCCA/MassSpring.git
// ------------------------------------------------------------------------------------------------------------

class State
{
    public:
        glm::vec3 m_position;
        glm::vec3  m_velocity;
        inline State(glm::vec3 _pos, glm::vec3 _vel) 
            : m_position(_pos), m_velocity(_vel) {}
        inline State() {}
};

/// @brief a simple base class to do RK4 integration this class then needs the motionFunction to be
/// implemented so we can integrate the values

class RK4Integrator
{
    public:
        RK4Integrator(State _state) 
            : m_state(_state) {}
        RK4Integrator() {}
        
        State evaluate(const State &_initial, float _t, glm::vec3 force);
        //----------------------------------------------------------------------------------------------------------------------
        /// @brief method to evaluate the state given initial value and time step
        /// @param[in] _initial the initial state
        /// @param[in] _t the current time step value
        /// @param[in] _dt the delta time step for integration
        /// @param[in] _d the derivative for the integration step
        //----------------------------------------------------------------------------------------------------------------------
        State evaluate(const State &_initial, float _t, float _dt, const State &_d, glm::vec3 force);
        //----------------------------------------------------------------------------------------------------------------------
        /// @brief method to evaluate the state given initial value and time step
        /// @param[in] _initial the initial state
        /// @param[in] _t the current time step value
        /// @param[in] _dt the delta time step for integration
        //----------------------------------------------------------------------------------------------------------------------
        void integrate(float _t, float _dt, glm::vec3 force);
        inline State getState() const { return m_state; }
    protected:
        State m_state;
};
