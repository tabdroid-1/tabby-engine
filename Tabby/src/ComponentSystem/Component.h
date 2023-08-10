#pragma once

class GameObject;

class Component {
public:
  Component(GameObject *owner) : owner(owner) {}

  virtual void Awake(){};
  virtual void Start(){};

  virtual void Update(float deltaTime){};
  virtual void LateUpdate(float deltaTime){};
  virtual void Draw(){};
  virtual void Delete(){};

protected:
  GameObject *owner;
};
