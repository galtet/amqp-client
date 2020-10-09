FROM centos:7

RUN yum install epel-release -y
RUN yum install make gcc lua-devel librabbitmq-devel librabbitmq luarocks luajit nc valgrind -y
RUN  mkdir /amqp-client/
COPY . /amqp-client/

workdir /amqp-client/
