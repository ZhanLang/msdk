#pragma once

//-------------------------------------------------------------------------------------
/**
    Zoom scale support.
    scale is a integer, if >= 1 means zoom in, if <= -1 means zoom out.
*/
class PCL_Interface_ZoomScale
{
    int   m_scale ;
public:
    PCL_Interface_ZoomScale() : m_scale(1) {}
    virtual ~PCL_Interface_ZoomScale() {}

    /// Set zoom scale.
    void SetZoomScale (int nScale)
    {
        assert(nScale) ;
        if (nScale)
        {
            m_scale = nScale ;
        }
    }

    /// Get zoom scale.
    int GetZoomScale() const {return m_scale;}

    /// @name Coordinate transformation
    //@{
    /// scaled --> actual.
    void Scaled_to_Actual (POINT& pt) const
    {
        if (m_scale > 1)
        {
            pt.x /= m_scale ;
            pt.y /= m_scale ;
        }
        else
            if (m_scale < -1)
            {
                pt.x *= -m_scale ;
                pt.y *= -m_scale ;
            }
    }

    /// actual --> scaled.
    void Actual_to_Scaled (POINT& pt) const
    {
        if (m_scale > 1)
        {
            pt.x *= m_scale ;
            pt.y *= m_scale ;
        }
        else
            if (m_scale < -1)
            {
                pt.x /= -m_scale ;
                pt.y /= -m_scale ;
            }
    }
    //@}
};
