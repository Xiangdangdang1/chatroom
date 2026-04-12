const path = require('path');
const grpc = require('@grpc/grpc-js');
const protoLoader = require('@grpc/proto-loader');
const PROTO_PATH = path.join(__dirname, 'message.proto');   //拼接路径
const packageDefinition = protoLoader.loadSync(PROTO_PATH, {    //同步加载proto文件
    keepCase: true,   //保持大小写区分
    longs: String,   //将long类型转换为字符串
    enums: String,   //将enum类型转换为字符串
    defaults: true,  //设置默认值
    oneofs: true     //支持oneof语法
});

//加载proto文件(转为对象)并生成对应的JavaScript对象
const protoDescriptor = grpc.loadPackageDefinition(packageDefinition);  
 
const message_proto = protoDescriptor.message;   //获取message包下的服务定义

module.exports = message_proto;   //导出message_proto对象供其他模块使用