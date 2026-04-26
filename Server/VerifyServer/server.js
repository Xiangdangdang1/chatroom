const grpc = require('@grpc/grpc-js');
const message_proto = require('./proto');
const emailModule = require('./email');
const const_module = require('./const');
const { v4: uuidv4 } = require('uuid');  //引入uuid库生成唯一id
const redis_module = require('./redis');



async function GetVerifyCode(call, callback) {
    console.log("email is ", call.request.email)
    try{
        //前缀(code_) + 邮箱地址作为key查询redis，判断是否存在
        let query_res = await redis_module.QueryRedis(const_module.code_prefix + call.request.email);
        query_res = query_res === true ? await redis_module.GetValue(const_module.code_prefix + call.request.email) : null;
        console.log("query res is ", query_res);
        let uniqueId = query_res;
        if (query_res === null){
            //如果不存在，生成唯一id作为验证码，并将验证码存入redis，设置过期时间为600秒
            uniqueId = uuidv4();
            if (uniqueId.length > 4)   //截取前4位作为验证码
            {
                uniqueId = uniqueId.substring(0, 4);
            }
            let bres = await redis_module.SetValueExpire(const_module.code_prefix + call.request.email, uniqueId, 600);
            if (!bres){
                //如果存入redis失败，返回错误信息
                callback(null, { email:  call.request.email,
                    error:const_module.Errors.RedisError
                }); 
                return;
            }
        }

        console.log("uniqueId is ", uniqueId)
        let text_str =  '您的验证码为'+ uniqueId +'请五分钟内完成注册'
        //发送邮件
        let mailOptions = {
            from: 'q510655254@163.com',
            to: call.request.email,
            subject: '验证码',
            text: text_str,
        };

        //await等待发送邮件的结果，此时执行SendMail的回调，返回结果
        let send_res = await emailModule.SendMail(mailOptions);
        console.log("send res is ", send_res)

        
        //如果发送邮件失败，返回错误信息
        callback(null, { email:  call.request.email,
            error:const_module.Errors.EmailError
        }); 

    }catch(error){ 
        console.log("catch error is ", error)
        callback(null, { email:  call.request.email,
            error:const_module.Errors.Exception
        }); 
    }
}
function main() {
    var server = new grpc.Server()  //启动grpc服务器
    //将服务注册到服务器上，GetVerifyCode函数作为服务的实现, (key,value)形式
    server.addService(message_proto.VerifyService.service, { GetVerifyCode: GetVerifyCode })
    server.bindAsync('0.0.0.0:50051', grpc.ServerCredentials.createInsecure(), () => {
        server.start()
        console.log('grpc server started')        
    })
}
main()