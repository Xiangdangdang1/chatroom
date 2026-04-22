const config_module = require('./config.js');
const Redis = require("ioredis");

//创建Redis客户端实例
const RedisClinet = new Redis({
    port: config_module.redis_port,          // Redis port
    host: config_module.redis_host,          // Redis host
    password: config_module.redis_passwd,    // Redis password
});

RedisClinet.on('error', function (err) {
    console.log('Redis client error:');
    RedisClinet.quit();
});

//根据key获取value
async function GetValue(key){
    try{
        const value = await RedisClinet.get(key);
        if (value === null){
            console.log("redis get value is null");
            return null;
        }
        console.log("redis get value is ", value);
        return value;
    }catch(error){
        console.log("redis get value error is ", error);
        return null;
    }
}

//查询key是否存在
async function QueryRedis(key){
    try{
        const result = await RedisClinet.exists(key);
        if (result === 0){
            console.log("redis exists result is 0");
            return null;
        }
        console.log("redis exists result is ", result);
        return result === 1;
    }catch(error){
        console.log("redis exists error is ", error);
        return null;
    }
}

//设置key和value，并设置过期时间
async function SetValueExpire(key, value, expire_time){
    try{
        await RedisClinet.set(key, value);
        await RedisClinet.expire(key, expire_time);
        return true;
    }catch(error){
        console.log("redis set value error is ", error);
        return false;
    }
}

//退出
function QuitRedis(){
    RedisClinet.quit();
}

module.exports = {GetValue, QueryRedis, SetValueExpire, QuitRedis}