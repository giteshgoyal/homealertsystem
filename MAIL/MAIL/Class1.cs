using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net.Mail;

namespace MAIL
{

public interface ICalculator
{
      void mailsms();
};

public class Class1 : ICalculator
    {

public void mailsms()
            {

                string toAddress = "giteshgoyal77@gmail.com";
                string subject = "TEst";
                string body = "Test body jdjhdjhedj";
                string result = " Message Sent Successfully..!!";
                string senderID = "alertsystem100@gmail.com";// use sender’s email id here..
                const string senderPassword = "alertsms@100"; // sender password here…

                try
                {
                    SmsClient.SendSms sms = new SmsClient.SendSms();
                    string status = sms.send("9463600306", "alertsms100", "SUCESS SMSSSS", "9463600306");
                    SmtpClient smtp = new SmtpClient
                    {
                        Host = "smtp.gmail.com", // smtp server address here…
                        Port = Convert.ToInt16("587"),
                        EnableSsl = true,
                        UseDefaultCredentials = false,
                        DeliveryMethod = SmtpDeliveryMethod.Network,
                        Credentials = new System.Net.NetworkCredential(senderID, senderPassword),
                        Timeout = 30000,
                    };
                    MailMessage message = new MailMessage(senderID, toAddress, subject, body);
                    System.Net.Mail.Attachment attachment;
                    attachment = new System.Net.Mail.Attachment("C:\\Users\\Gitesh\\Documents\\Visual Studio 2015\\Projects\\motiontracking\\motiontracking\\captured.jpg");
               
                message.Attachments.Add(attachment);
             
                smtp.Send(message);



                     System.Console.WriteLine(result);
                    System.Console.WriteLine(status);
                }
                catch (System.Exception ex)
                {
                    result = "Error sending email.!!!";
                    System.Console.WriteLine(result);
                }
                // return result;
            }




        }
    }




