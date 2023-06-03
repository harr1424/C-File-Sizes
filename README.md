# C-File-Sizes
A simple program written in C to find large files in a specified directory. 

This project has two branches: [the main branch](https://github.com/harr1424/c_filesystem_size/tree/main/src) uses dynamic memory allocation to add filesyste entries to an ever-growing array. [The static-allocation branch](https://github.com/harr1424/c_filesystem_size/tree/static-allocation/src) uses a statically allocated array of ten filesystem entries, and will replace existing entries with larger ones as they are found. 

Benchmarking the two implementations against the directory containing the [Google Cloud CLI](https://cloud.google.com/sdk/docs/install-sdk) shows that the statically allocated array implementation offers only a slight improvement in program runtime. 

```
Finding 10 largest files in: /Users/user/google-cloud-sdk
/Users/user/google-cloud-sdk/bin/anthoscli      94968320
/Users/user/google-cloud-sdk/data/cli/gcloud.json       71649707
/Users/user/google-cloud-sdk/lib/googlecloudsdk/generated_clients/apis/compute_alpha.json       4728478
/Users/user/google-cloud-sdk/lib/googlecloudsdk/generated_clients/apis/compute_beta.json        4147516
/Users/user/google-cloud-sdk/lib/googlecloudsdk/generated_clients/apis/compute/alpha/compute_alpha_messages.py  4034669
/Users/user/google-cloud-sdk/lib/googlecloudsdk/generated_clients/apis/compute_v1.json  3904144
/Users/user/google-cloud-sdk/data/cli/gcloud_completions.py     3754693
/Users/user/google-cloud-sdk/lib/googlecloudsdk/generated_clients/apis/compute/beta/compute_beta_messages.py    3537222
/Users/user/google-cloud-sdk/lib/googlecloudsdk/generated_clients/apis/compute/v1/__pycache__/compute_v1_messages.cpython-37.pyc        3388318
/Users/user/google-cloud-sdk/lib/googlecloudsdk/generated_clients/apis/compute/v1/compute_v1_messages.py        3360648
```

I created similar programs using [C++](https://github.com/harr1424/cpp_filesystem_size), [Go](https://github.com/harr1424/go_filesystem_size), and [Rust](https://github.com/harr1424/hello_rust). 
