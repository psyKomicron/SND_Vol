#pragma once

class IComEventImplementation
{
    /**
     * @brief Register the com event implementation for events.
     * @return True if the implementation registered successfully
    */
    virtual bool Register() PURE;

    /**
     * @brief Unregister the com event implementation from events.
     * @return True if the implementation unregistered successfully
    */
    virtual bool Unregister() PURE;
};

