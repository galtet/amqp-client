FROM centos:7

RUN rpm -ivh http://repo.okay.com.mx/centos/7/x86_64/release/okay-release-1-3.el7.noarch.rpm
RUN yum install epel-release -y
RUN yum install make gcc lua-devel librabbitmq-devel librabbitmq9 luarocks luajit nc valgrind -y
RUN  mkdir /amqp-client/
COPY . /amqp-client/

workdir /amqp-client/
