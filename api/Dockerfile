# Using debian bullseye
FROM debian:bullseye
MAINTAINER castile

# Installs
RUN apt update
RUN apt install -y curl=7.74.0-1.3+deb11u1
RUN apt install -y python3-pip=20.3.4-4+deb11u1
RUN apt install -y net-tools #debug
RUN apt install -y tcpdump #debug
RUN pip install flask==2.0.1

# Add API
ADD ./api.py /api.py
EXPOSE 5000
ENV FLASK_APP=api.py

# Run
ENTRYPOINT ["python3"]
CMD ["/api.py"]
