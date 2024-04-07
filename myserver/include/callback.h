#pragma once

#include <memory>
#include <functional>

class Buffer;
class TcpConnection;
class Timestamp;

// 原先很多都是在 net中的， 我提取了出来， 但是 头文件不知道行不行

// using 首选， typedef 次选， #define 不要用

// 只有头，没有cpp  值是 类型别名

using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
using ConnectionCallback = std::function<void (const TcpConnectionPtr&)>;
using CloseCallback = std::function<void (const TcpConnectionPtr&)>;
using WriteCompleteCallback = std::function<void (const TcpConnectionPtr&)>; // 感觉 应该叫WriteCompletionCallback
using HighWaterMarkCallback = std::function<void (const TcpConnectionPtr&, size_t)>;
using MessageCallback = std::function<void (const TcpConnectionPtr&, Buffer*, Timestamp)>;

