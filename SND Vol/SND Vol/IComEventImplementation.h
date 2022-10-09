#pragma once

class IComEventImplementation
{
public:
    /// <summary>
    /// Register the com event implementation for events.
    /// </summary>
    /// <returns>True if the implementation registered successfully</returns>
    virtual bool Register() PURE;
    /// <summary>
    /// Unregister the com event implementation from events.
    /// </summary>
    /// <returns>True if the implementation unregistered successfully</returns>
    virtual bool Unregister() PURE;
};

