import yaml

def read_yaml_file(file_path):
    with open(file_path, 'r') as file:
        data = yaml.safe_load(file)
    return data

def write_yaml_file(file_path, data):
    with open(file_path, 'w+') as file:
        yaml.dump(data, file, indent=4, sort_keys=False)