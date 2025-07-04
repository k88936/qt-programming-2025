# TextureManager
TextureManager is a singleton class for loading and managing textures.

basic api:
```cpp
Texture * getTexture(std::string dir,int index)
```
params:  
- dir is the directory of the texture(s)
- index is the index of the texture(no naming style is must be followed ,so the index means the order of file name  ,e.g. 0.png,1.png,2.png...  dog_0.png,dog_1.png,dog_2.png...)
this function first check cache, if not exist, load from dir and add to cache ,load the whole dir but only return the one needed

