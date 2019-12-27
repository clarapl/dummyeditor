#pragma once

namespace DrawingTool {
class Selection;
namespace Blocking {
class Eraser;
class Pen;
} // namespace Blocking
namespace Graphic {
class Pen;
class Rectangle;
class Eraser;
class Selection;
} // namespace Graphic

class Visitor {
public:
    virtual void visitTool(Graphic::Pen&) = 0;
    virtual void visitTool(Graphic::Rectangle&) = 0;
    virtual void visitTool(Graphic::Eraser&) = 0;
    virtual void visitTool(Selection&) = 0;
    virtual void visitTool(Blocking::Pen&) = 0;
    virtual void visitTool(Blocking::Eraser&) = 0;
};

} // namespace DrawingTool
