use crate::server::error::ClientError;
use crate::server::kapi;
use crate::server::kapi::RestResponse as _;
use crate::server::result::Result;

async fn post<Req>(client: &reqwest::Client, req: Req) -> Result<reqwest::Response>
where
    Req: kapi::RestRequest,
{
    // TODO: Make the host/port configurable!
    let url = format!("http://localhost:3088/{}", Req::path());
    client
        .post(url)
        .body(serde_json::to_string(&req)?)
        .send()
        .await
        .map_err(ClientError::ReqwestError)?
        .error_for_status()
        .map_err(ClientError::ReqwestError)
}

pub async fn register(client: &reqwest::Client, req: kapi::RegisterRequest) -> Result<()> {
    post(client, req).await?;
    Ok(())
}

pub async fn login(
    client: &reqwest::Client,
    req: kapi::LoginRequest,
) -> Result<kapi::LoginResponse> {
    let res = post(client, req).await?;
    kapi::LoginResponse::from_str(&res.text().await?)
}
