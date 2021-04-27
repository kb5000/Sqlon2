# Sqlon 2

Sqlon 2是一个类JSON的通用的序列化库，生成的文件分为Schema和Data两部分。其数据模型如下：


| Sqlon 2类型 | C++类型 | 容器类型 |
| - | - | - |
| Null | / |   |
| Bool | bool |   |
| Int | int64_t |   |
| Double | double |   |
| String | string | 字节序列 |
| List | vector<Sqlon> | 后接多个Sqlon 2序列 |
| Object | Map<String, Sqlon> | 后接一系列键值对 |
| Array | vector<Sqlon> | 后接一个Sqlon 2序列的重复 |

设计思想：数据模型由Sqlon 1的仿JSON架构演化而来，在一些地方做了变更，使其更加具有解释性和可用性。

1. List和Object固定为不可变长，也就是长度信息储存在Schema中而非Data中，因为存储在Data是毫无意义的。Object的固定长度Key也去掉，虽然其可以降低占用空间但基本无法使用。
2. 增加了Array类型，后面只跟一个Sqlon序列，但可以根据Data中的长度信息重复多次，这使得Sqlon的Schema可以表示变长数据，是真正的变长功能，不再需要愚蠢的L2SS了。

### Schema定义


| Sqlon 2类型 | Schema格式 |
| - | - |
| Null | N |
| Bool | B |
| Int | I |
| Double | D |
| String | S\<Length> |
| List | L[Length]+ |
| Object | O[Length]+ |
| Array | A- |

其中，尖括号代表可选项，方括号代表必选项，+代表此处可以跟Length个Sqlon 2序列，-代表此处可以跟1个Sqlon 2序列。

### Data定义


| Sqlon 2类型 | Data格式 | 空间 |
| - | - | - |
| Null | 不存储 | 0 |
| Int | 小端序 | 8字节 |
| Double | 小端序 | 8字节 |
| String | 如果是可变长的先8字节Int型长度后接字节序列 | 变长 |
| List | 存储后面的Sqlon 2序列 | 0（本身不占空间） |
| Object | 对于每一项，先存储一个String作为Key再存储一个Sqlon 2作为Value | 变长 |
| Array | 先8字节长度再存储多个Sqlon 2序列 | 变长 |

### 内存模型：

Sqlon 2保存的数据是一个树形的结构。如一个List型数据中含有一个vector\<Sqlon2\>类型的指针，保存的内容就是具体的List数据。我们可以用getList()方法来访问其中的数据，也就是返回一个vector\<Sqlon2\>的引用。

Sqlon 2采用类似vector的指针模式，也就是Sqlon 2保存一个对象的独占所有权，在发生复制的时候会自动进行深拷贝。

### API

#### C++

```cpp
Sqlon2(), Sqlon2(bool), Sqlon2(int), Sqlon2(double), Sqlon2(string), Sqlon2(vector<Sqlon2>), Sqlon2(map<string, Sqlon2>)
```

通过原始类型构建一个Sqlon2对象，进行复制操作。

对于`string`参数，其签名为`Sqlon2(string, bool varval=true)`，第二个参数如果为false则表示长度信息保存在Schema中。

对于`vector<Sqlon2>`参数，其签名为`Sqlon2(vector<Sqlon2>, bool isArray=false)`，第二个参数如果为true则表示创建的是一个Array而非List。

```cpp
Sqlon2 obj;
obj.isNull();
obj.getBool();
obj.getInt();
obj.getDouble();
obj.getString();
obj.getList();
obj.getObjet();
obj.get<T>();
```

返回Sqlon2对象包含的数据，返回的是引用。

```cpp
Sqlon2 a, b;
a = b;
a == b;
a < b;
a > b;
a <= b;
a >= b;
a != b;
```

Sqlon 2实现了一个偏序关系，也就是同一类型的两数据如果是Bool, Int, Double, String是可以比较的，而不同类型数据无论如何比较结果都是false。同时，List的比较可以自动按照字典序比较其中的元素，如果长度不同那么无法比较。

关于Null的比较，可以认为Null是一个值，因此可以判断相等。

```cpp
Sqlon2 obj;
obj.serialize();
obj.describe();
Sqlon::deserialize(data, description);
```

Sqlon 2的核心功能是序列化。`serialize`和`describe`要分开生成。`serialize`得到的是二进制字符串，而`describe`得到的是人类可读的字符串。最终反序列化时同时需要两个数据。

```cpp
Sqlon2 obj;
obj.getTypeName();
obj.to_string();
```

Sqlon 2还提供了元信息功能方便调试。getTypeName返回其数据类型的String，而to_string返回其JSON格式的可读字符串。

### 注意

Array功能目前还不完善，必须手动保证Array内部的所有数据都是同一结构的，不然序列化会出错。
