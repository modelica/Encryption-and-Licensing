FROM python
RUN apt update && apt install -y gdb
RUN pip install cmake
RUN adduser dev
USER dev
