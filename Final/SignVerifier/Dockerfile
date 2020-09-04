FROM ubuntu:18.04

# Setup environ
ENV user signverifier
ENV prob_port 12354

# Install packages
RUN apt-get update 
RUN apt-get install -y tar socat python3

# Change permission
RUN chmod 1733 /tmp /var/tmp /dev/shm

# Additional configuration
RUN adduser $user
ADD ./binary_flag/openjdk-14.0.2_linux-x64_bin.tar.gz /home/$user/
ADD ./binary_flag/run.sh /home/$user/run.sh
ADD ./binary_flag/run.py /home/$user/run.py
ADD ./binary_flag/bin.tar.gz /home/$user/
ADD ./binary_flag/signature_verify_key /home/$user/signature_verify_key
ADD ./binary_flag/flag /home/$user/flag

RUN chown -R root:root /home/$user/
RUN chown root:$user /home/$user/run.sh
RUN chown root:$user /home/$user/run.py
RUN chown root:$user /home/$user/SignVerifier.jar
RUN chown root:$user /home/$user/app-cds.jsa
RUN chown root:$user /home/$user/libSign.so
RUN chown root:$user /home/$user/signature_verify_key
RUN chown root:$user /home/$user/jdk-14.0.2
RUN chown root:$user /home/$user/flag

RUN chmod 2755 /home/$user/run.sh
RUN chmod 2755 /home/$user/run.py
RUN chmod a-w -R /home/$user/jdk-14.0.2
RUN chmod 440 /home/$user/SignVerifier.jar
RUN chmod 440 /home/$user/app-cds.jsa
RUN chmod 440 /home/$user/libSign.so
RUN chmod 440 /home/$user/signature_verify_key
RUN chmod 440 /home/$user/flag

# final
WORKDIR /home/$user
CMD socat -T 120 TCP-LISTEN:$prob_port,reuseaddr,fork EXEC:/home/$user/run.py
USER $user
EXPOSE $prob_port