# Copyright 2022 Zontec
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# syntax=docker/dockerfile:1

FROM gcc:latest

RUN apt-get update
RUN apt-get install doxygen -y
RUN apt-get install python3 python3-pip -y
RUN pip install numpy

COPY ./ /hsm
WORKDIR /hsm/

RUN make clean
RUN make
WORKDIR /hsm

CMD ./build/phsm.out