#include <CustomComponent/PlayerComponent.h>

template <>
void Tabby::World::OnComponentAdded<PlayerComponent>(Entity entity, PlayerComponent& component)
{

    TB_INFO("PlayerComponent Added");
}
