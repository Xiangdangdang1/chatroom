const grpc = require('@grpc/grpc-js');
const message_proto = require('./proto');
const emailModule = require('./email');
const const_module = require('./const');
const { v4: uuidv4 } = require('uuid');  //引入uuid库生成唯一id


async function GetVerifyCode(call, callback) {
    console.log("email is ", call.request.email)
    try{
        uniqueId = uuidv4();
        console.log("uniqueId is ", uniqueId)
        let text_str =  '您的验证码为'+ uniqueId +'请三分钟内完成注册'
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
        callback(null, { email:  call.request.email,
            error:const_module.Errors.Success
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