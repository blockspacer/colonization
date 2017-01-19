#include <Colonization/BuildConfiguration.hpp>
#include "ImageUtils.hpp"
#include <Urho3D/IO/Log.h>
#include <cmath>

namespace Colonization
{
namespace ImageUtils
{
void DrawLine (Urho3D::Image *image, Urho3D::Color color, int x1, int y1, int x2, int y2, int width)
{
    float xDiff = x2 - x1;
    float yDiff = y2 - y1;

    if (xDiff == 0.0f && yDiff == 0.0f)
    {
        image->SetPixel (x1, y1, color);
        return;
    }

    if (Urho3D::Abs (xDiff) > Urho3D::Abs (yDiff))
    {
        float xMax;
        float xMin;

        if (x1 > x2)
        {
            xMax = x1;
            xMin = x2;
        }
        else
        {
            xMax = x2;
            xMin = x1;
        }

        float tangent = yDiff / xDiff;
        for (float x = xMin; x <= xMax; x += width)
        {
            float y = y1 + (x - x1) * tangent;
            if (width <= 1)
                image->SetPixel (x, y, color);
            else
            {
                int halfWidth = static_cast <int> (std::floor (width * 0.5f));
                for (int xOffset = -halfWidth; xOffset <= halfWidth; xOffset++)
                    for (int yOffset = -halfWidth; yOffset <= halfWidth; yOffset++)
                        image->SetPixel (x + xOffset, y + yOffset, color);
            }
        }
    }
    else
    {
        float yMax;
        float yMin;

        if (y1 > y2)
        {
            yMax = y1;
            yMin = y2;
        }
        else
        {
            yMax = y2;
            yMin = y1;
        }

        float tangent = xDiff / yDiff;
        for (float y = yMin; y <= yMax; y += 1.0f)
        {
            float x = x1 + (y - y1) * tangent;
            if (width <= 1)
                image->SetPixel (x, y, color);
            else
            {
                int halfWidth = static_cast <int> (std::floor (width * 0.5f));
                for (int xOffset = -halfWidth; xOffset <= halfWidth; xOffset++)
                    for (int yOffset = -halfWidth; yOffset <= halfWidth; yOffset++)
                        image->SetPixel (x + xOffset, y + yOffset, color);
            }
        }
    }
}

void DrawCircle (Urho3D::Image *image, Urho3D::Color color, int x, int y, int radius, bool fill)
{
    int drawX = radius;
    int drawY = 0;
    int error = 0;

    while (drawX >= drawY)
    {
        if (fill)
        {
            DrawLine (image, color, x - drawX, y + drawY, x + drawX, y + drawY, 1);
            DrawLine (image, color, x - drawY, y + drawX, x + drawY, y + drawX, 1);
            DrawLine (image, color, x - drawY, y - drawX, x + drawY, y - drawX, 1);
            DrawLine (image, color, x - drawX, y - drawY, x + drawX, y - drawY, 1);
        }
        else
        {
            image->SetPixel (x + drawX, y + drawY, color);
            image->SetPixel (x + drawY, y + drawX, color);
            image->SetPixel (x - drawY, y + drawX, color);
            image->SetPixel (x - drawX, y + drawY, color);
            image->SetPixel (x - drawX, y - drawY, color);
            image->SetPixel (x - drawY, y - drawX, color);
            image->SetPixel (x + drawY, y - drawX, color);
            image->SetPixel (x + drawX, y - drawY, color);
        }

        if (error <= 0)
        {
            drawY += 1;
            error += 2 * drawY + 1;
        }

        if (error > 0)
        {
            drawX -= 1;
            error -= 2 * drawX + 1;
        }
    }
}

void FloodFill (Urho3D::Image *image, Urho3D::Color color, int x, int y)
{
    unsigned sourceColor = image->GetPixelInt (x, y);
    unsigned fillColor = color.ToUInt ();

    // FIXME: Current implementation is VERY slow! Fix it. Maybe use lines algorithm or something else.
    if (sourceColor != fillColor)
    {
        Urho3D::PODVector <Urho3D::IntVector2> toSet;
        toSet.Push (Urho3D::IntVector2 (x, y));
        int index = 0;

        while (index < toSet.Size ())
        {
            Urho3D::IntVector2 point = toSet.At (index);
            Urho3D::Log::Write (Urho3D::LOG_INFO, Urho3D::String (point.x_) + " " + Urho3D::String (point.y_));
            if (image->GetPixelInt (point.x_, point.y_) == sourceColor)
            {
                image->SetPixelInt (point.x_, point.y_, fillColor);

                if (image->GetPixelInt (point.x_ - 1, point.y_) == sourceColor)
                    toSet.Push (Urho3D::IntVector2 (point.x_ - 1, point.y_));

                if (image->GetPixelInt (point.x_ + 1, point.y_) == sourceColor)
                    toSet.Push (Urho3D::IntVector2 (point.x_ + 1, point.y_));

                if (image->GetPixelInt (point.x_, point.y_ - 1) == sourceColor)
                    toSet.Push (Urho3D::IntVector2 (point.x_, point.y_ - 1));

                if (image->GetPixelInt (point.x_, point.y_ + 1) == sourceColor)
                    toSet.Push (Urho3D::IntVector2 (point.x_, point.y_ + 1));
            }
            index++;
        }
    }
}
}
}

