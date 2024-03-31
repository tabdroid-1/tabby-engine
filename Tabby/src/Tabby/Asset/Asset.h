#pragma once

namespace Tabby {

class Asset {
private:
    Asset();

    virtual void Load();
    virtual void Unload();
};

}
