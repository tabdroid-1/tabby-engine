#pragma once

class C_Drawable {
public:
    C_Drawable();
    virtual ~C_Drawable();

    virtual void Draw() = 0;

    void SetSortOrder(int order);
    int GetSortOrder() const;

private:
    int sortOrder;
};
