//Sphere Shader

#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

out vec3 v_Normal;
out vec3 v_FragPos;

void main()
{
    vec4 worldPos = u_Transform * vec4(a_Position, 1.0);
    v_FragPos = worldPos.xyz;
    v_Normal = mat3(transpose(inverse(u_Transform))) * a_Normal;

    gl_Position = u_ViewProjection * worldPos;
}

#type fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec3 v_Normal;
in vec3 v_FragPos;

uniform vec4 u_Color;

void main()
{
    // Simple lighting calculation
    vec3 norm = normalize(v_Normal);
    vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0)); // Simple directional light

    // Ambient
    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * u_Color.rgb;

    // Diffuse
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * u_Color.rgb * 0.7;

    vec3 result = ambient + diffuse;
    color = vec4(result, u_Color.a);
}