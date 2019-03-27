FROM zbeekman/nightly-gcc-trunk-docker-image
LABEL maintainer="boris.rura@gmail.com"


USER root
# java install fails because of some weird update-alternatives error, this fixes the issue.
RUN mkdir -p /usr/share/man/man1 
RUN apt -y update
RUN apt -y install apt-utils flex re2c curl default-jdk
RUN curl https://www.antlr.org/download/antlr-4.7.2-complete.jar -o antlr.jar && mv antlr.jar /usr/local/lib

RUN useradd -ms /bin/bash develop -p $(openssl passwd -1 "develop")
RUN echo "develop   ALL=(ALL:ALL) ALL" >> /etc/sudoers

# for gdbserver
EXPOSE 2000

USER develop
RUN echo "export CLASSPATH=\".:/usr/local/lib/antlr.jar:$CLASSPATH\"" >> ~/.bashrc \
    && echo "alias antlr4='java -Xmx500M -cp \"/usr/local/lib/antlr.jar:$CLASSPATH\" org.antlr.v4.Tool'" >> ~/.bash_aliases \
    && . ~/.bashrc

RUN curl https://www.antlr.org/download/antlr4-cpp-runtime-4.7.2-source.zip -o 
VOLUME "/home/develop/project"
WORKDIR "/home/develop/project"